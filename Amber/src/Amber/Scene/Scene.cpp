#include "abpch.h"
#include "Scene.h"

#include <Box2D/box2d.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Amber/Math/Transforms.h"

#include "Amber/Renderer/SceneRenderer.h"
#include "Amber/Renderer/Renderer.h"
#include "Amber/Renderer/Renderer2D.h"

#include "Amber/Scene/Components.h"
#include "Amber/Scene/Entity.h"
#include "Amber/Scene/SceneCamera.h"

#include "Amber/Script/ScriptEngine.h"

namespace Amber
{

static uint32_t s_SceneCounter = 0;
std::unordered_map<UUID, Scene*> g_ActiveScenes;

struct SceneComponent
{
    UUID SceneID;

    operator UUID& () { return SceneID; }
    operator const UUID& () const { return SceneID; }
};

struct Box2DWorldComponent
{
    Scope<b2World> World;
};

static b2BodyType AmberRigidBodyTypeToBox2DType(RigidBody2DComponent::Type type)
{
    switch (type)
    {
        case Amber::RigidBody2DComponent::Type::Static:     return b2_staticBody;
        case Amber::RigidBody2DComponent::Type::Dynamic:    return b2_dynamicBody;
        case Amber::RigidBody2DComponent::Type::Kinematic:  return b2_kinematicBody;
    }

    AB_CORE_ASSERT(false, "Invalid rigid body type!");
    return b2_staticBody;
}

void OnScriptConstruct(entt::registry& registry, entt::entity entity)
{
    auto view = registry.view<SceneComponent>();
    UUID sceneID = registry.get<SceneComponent>(view.front()).SceneID;

    Scene* scene = g_ActiveScenes[sceneID];

    auto entityID = registry.get<IDComponent>(entity);
    ScriptEngine::InitScriptEntity(scene->GetEntityMap().at(entityID));
}

Environment Environment::Load(const std::string& filepath)
{
    auto [irradiance, radiance] = SceneRenderer::CreateEnvironmentMap(filepath);
    return { filepath, irradiance, radiance };
}

Scene::Scene(const std::string& debugName, const std::string& assetPath)
    : m_DebugName(debugName), m_AssetPath(assetPath)
{
    m_Registry.on_construct<ScriptComponent>().connect<&OnScriptConstruct>();

    m_SceneEntity = m_Registry.create();
    m_Registry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);
    m_Registry.emplace<Box2DWorldComponent>(m_SceneEntity, CreateScope<b2World>(b2Vec2(0.0f, -9.81f)));

    g_ActiveScenes[m_SceneID] = this;

    Init();
}

Scene::~Scene()
{
    m_Registry.clear();
    g_ActiveScenes.erase(m_SceneID);
    ScriptEngine::OnSceneDestruct(m_SceneID);
}

void Scene::Init()
{
    auto skyboxShader = Shader::Create("assets/shaders/Skybox.glsl");
    m_SkyboxMaterial = Ref<MaterialInstance>::Create(Ref<Material>::Create(skyboxShader));
    m_SkyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);
}

void Scene::OnUpdate(Timestep ts)
{
    auto entities = GetAllEntitiesWith<ScriptComponent>();
    for (auto entity : entities)
    {
        UUID entityID = m_Registry.get<IDComponent>(entity);
        Entity e = { entity, this };
        if (ScriptEngine::ModuleExists(e.GetComponent<ScriptComponent>().ModuleName))
            ScriptEngine::OnUpdateEntity(m_SceneID, entityID, ts);
    }

    auto& box2DWorld = m_Registry.get<Box2DWorldComponent>(m_SceneEntity).World;
    int32_t velocityIterations = 8;
    int32_t positionIterations = 3;
    box2DWorld->Step(ts, velocityIterations, positionIterations);

    auto rigidBodies = m_Registry.view<RigidBody2DComponent>();
    for (auto e : rigidBodies)
    {
        Entity entity(e, this);
        auto& transform = entity.Transform();
        const auto& rigidBody2D = entity.GetComponent<RigidBody2DComponent>();
        b2Body* body = static_cast<b2Body*>(rigidBody2D.RuntimeBody);

        auto& position = body->GetPosition();
        auto [translation, rotationQuat, scale] = Math::DecomposeTransform(transform);
        glm::vec3 rotation = glm::eulerAngles(rotationQuat);

        transform = glm::translate(glm::mat4(1.0f), { position.x, position.y, translation.z }) *
            glm::toMat4(glm::quat({ rotation.x, rotation.y, body->GetAngle() })) *
            glm::scale(glm::mat4(1.0f), scale);
    }
}

void Scene::OnRenderEditor(Timestep ts, const EditorCamera& camera, std::vector<Entity>& selectionContext)
{
    m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLOD);

    Entity sceneCameraEntity;
    std::vector<Entity> cameraEntities;
    for (auto& entity : selectionContext)
    {
        if (entity.HasComponent<CameraComponent>())
        {
            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            cameraEntities.push_back(entity);
        }
    }
    if (!cameraEntities.empty())
        sceneCameraEntity = *cameraEntities.rbegin();

    Ref<Texture2D> cameraPreview = nullptr;
    if (sceneCameraEntity)
    {
        OnRenderRuntime(ts, &sceneCameraEntity);

        auto& colorBuffer = SceneRenderer::GetFinalColorBuffer();
        cameraPreview = Texture2D::Create(
            colorBuffer->GetFormat(), colorBuffer->GetWidth(), colorBuffer->GetHeight());
        colorBuffer->CopyTo(cameraPreview);
    }

    SceneRenderer::BeginScene(this, { camera, camera.GetViewMatrix() });

    auto entities = m_Registry.group<MeshComponent, TransformComponent>();
    for (auto entity : entities)
    {
        auto [meshComponent, transformComponent] = entities.get<MeshComponent, TransformComponent>(entity);
        if (meshComponent.Mesh)
        {
            meshComponent.Mesh->OnUpdate(ts);
            auto res = std::find(selectionContext.begin(), selectionContext.end(), entity);
            if (res != selectionContext.end())
                SceneRenderer::SubmitSelectedMesh(meshComponent, transformComponent);
            else
                SceneRenderer::SubmitMesh(meshComponent, transformComponent);
        }
    }

    for (auto& entity : cameraEntities)
    {
        auto& [cameraComponent, transformComponent] = entity.GetComponent<CameraComponent, TransformComponent>();
        SceneRenderer::SubmitCamera(cameraComponent, transformComponent);
    }

    SceneRenderer::EndScene();

    if (sceneCameraEntity)
    {
        auto renderPass = SceneRenderer::GetFinalRenderPass();
        Renderer::BeginRenderPass(renderPass, false);
        Renderer2D::BeginScene(glm::mat4(1.0f));

        static auto previewMaterial = Ref<MaterialInstance>::Create(Ref<Material>::Create(Renderer::GetShaderLibrary()->Get(ShaderType::UnlitTexture)));
        previewMaterial->SetFlag(MaterialFlag::DepthTest, false);
        previewMaterial->Set("u_ViewProjection", glm::mat4(1.0f));

        glm::vec3 position(0.75f, -0.75f, 0.0f);
        previewMaterial->Set("u_AlbedoTexture", cameraPreview);
        previewMaterial->Set("u_Transform", glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(0.2f, 0.2f, 1.0f)));

        RenderCommand::SetStencilFunction(ComparisonFunc::Always, 2, 0xff);
        RenderCommand::SetStencilMask(0xff);
        RenderCommand::SetStencilOperation(StencilOperation::Keep, StencilOperation::Keep, StencilOperation::Replace);

        Renderer2D::DrawFullscreenQuad(previewMaterial);

        static auto outlineMaterial = Ref<MaterialInstance>::Create(Ref<Material>::Create(SceneRenderer::GetShaderLibrary()->Get("Outline")));
        outlineMaterial->SetFlag(MaterialFlag::DepthTest, false);
        outlineMaterial->Set("u_Color", glm::vec3(0.8f));
        outlineMaterial->Set("u_ViewProjection", glm::mat4(1.0f));

        glm::vec2 size(0.21f, 0.21f);
        outlineMaterial->Set("u_Transform", glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(size, 0.0f)));
        outlineMaterial->Set("u_Center", position);
        outlineMaterial->Set("u_Size", size);
        outlineMaterial->Set("u_Radius", 0.145f);
        outlineMaterial->Set("u_Border", true);

        RenderCommand::SetStencilFunction(ComparisonFunc::NotEqual, 2, 0xff);
        RenderCommand::SetStencilMask(0);

        Renderer2D::DrawFullscreenQuad(outlineMaterial);

        RenderCommand::SetStencilFunction(ComparisonFunc::Always, 0, 0xff);
        RenderCommand::SetStencilMask(0xff);

        Renderer2D::EndScene();
        Renderer::EndRenderPass();
    }
}

void Scene::OnRenderRuntime(Timestep ts, Entity* sceneCameraEntity)
{
    m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLOD);

    Entity cameraEntity = sceneCameraEntity ? *sceneCameraEntity : GetMainCameraEntity();
    AB_CORE_ASSERT(cameraEntity, "Scene does not contain cameras!");

    glm::mat4 viewMatrix = glm::inverse(cameraEntity.GetComponent<TransformComponent>().Transform);
    SceneCamera& camera = cameraEntity.GetComponent<CameraComponent>();
    camera.Update();

    SceneRenderer::BeginScene(this, { camera, viewMatrix });

    auto entities = m_Registry.group<MeshComponent, TransformComponent>();
    for (auto entity : entities)
    {
        auto [meshComponent, transformComponent] = entities.get<MeshComponent, TransformComponent>(entity);
        if (meshComponent.Mesh)
        {
            meshComponent.Mesh->OnUpdate(ts);
            SceneRenderer::SubmitMesh(meshComponent, transformComponent);
        }
    }

    SceneRenderer::EndScene();
}

void Scene::OnRuntimeStart()
{
    ScriptEngine::SetSceneContext(this);

    auto entities = GetAllEntitiesWith<ScriptComponent>();
    for (auto entity : entities)
    {
        Entity e = { entity, this };
        if (ScriptEngine::ModuleExists(e.GetComponent<ScriptComponent>().ModuleName))
            ScriptEngine::InstantiateEntityClass(e);
    }

    auto& world = m_Registry.get<Box2DWorldComponent>(m_SceneEntity).World;
    auto rigidBodies = m_Registry.view<RigidBody2DComponent>();
    for (auto e : rigidBodies)
    {
        Entity entity(e, this);
        auto transform = entity.GetTransform();
        auto& rigidBody2D = entity.GetComponent<RigidBody2DComponent>();

        b2BodyDef bodyDef;
        bodyDef.type = AmberRigidBodyTypeToBox2DType(rigidBody2D.BodyType);

        auto [translation, rotationQuat, scale] = Math::DecomposeTransform(transform);
        bodyDef.position.Set(translation.x, translation.y);
        bodyDef.angle = glm::eulerAngles(rotationQuat).z;

        rigidBody2D.RuntimeBody = world->CreateBody(&bodyDef);
    }

    auto boxColliders = m_Registry.view<BoxCollider2DComponent>();
    for (auto e : boxColliders)
    {
        Entity entity(e, this);
        auto transform = entity.GetTransform();
        auto& boxCollider2D = entity.GetComponent<BoxCollider2DComponent>();

        auto rigidBody = entity.GetComponentIfExists<RigidBody2DComponent>();
        if (rigidBody)
        {
            AB_CORE_ASSERT(rigidBody->RuntimeBody, "Runtime body not initialized!");
            b2Body* body = static_cast<b2Body*>(rigidBody->RuntimeBody);

            b2PolygonShape polygonShape;
            polygonShape.SetAsBox(boxCollider2D.Size.x, boxCollider2D.Size.y);
            polygonShape.m_centroid.x += boxCollider2D.Offset.x;
            polygonShape.m_centroid.y += boxCollider2D.Offset.y;

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &polygonShape;
            fixtureDef.density = rigidBody->Density;
            fixtureDef.friction = rigidBody->Friction;
            boxCollider2D.RuntimeFixture = body->CreateFixture(&fixtureDef);
        }
    }

    auto circleColliders = m_Registry.view<CircleCollider2DComponent>();
    for (auto e : circleColliders)
    {
        Entity entity(e, this);
        auto transform = entity.GetTransform();
        auto& circleCollider2D = entity.GetComponent<CircleCollider2DComponent>();

        auto rigidBody = entity.GetComponentIfExists<RigidBody2DComponent>();
        if (rigidBody)
        {
            AB_CORE_ASSERT(rigidBody->RuntimeBody, "Runtime body not initialized!");
            b2Body* body = static_cast<b2Body*>(rigidBody->RuntimeBody);

            b2CircleShape circleShape;
            circleShape.m_radius = circleCollider2D.Radius;
            circleShape.m_p.x += circleCollider2D.Offset.x;
            circleShape.m_p.y += circleCollider2D.Offset.y;

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &circleShape;
            fixtureDef.density = rigidBody->Density;
            fixtureDef.friction = rigidBody->Friction;
            circleCollider2D.RuntimeFixture = body->CreateFixture(&fixtureDef);
        }
    }

    m_IsPlaying = true;
}

void Scene::OnRuntimeStop()
{
    auto entities = GetAllEntitiesWith<ScriptComponent>();
    for (auto entity : entities)
    {
        Entity e = { entity, this };
        if (ScriptEngine::ModuleExists(e.GetComponent<ScriptComponent>().ModuleName))
            ScriptEngine::OnDestroyEntity(m_SceneID, e.GetUUID());
    }
    
    m_IsPlaying = false;
}

void Scene::OnEvent(Event& e)
{
}

void Scene::SetEnvironment(const Environment& environment)
{
    m_Environment = environment;
    SetSkybox(environment.RadianceMap);
}

void Scene::SetSkybox(const Ref<TextureCube>& skybox)
{
    m_Skybox = skybox;
    m_SkyboxMaterial->Set("u_Texture", skybox);
}

float Scene::GetPhysics2DGravity() const
{
    return m_Registry.get<Box2DWorldComponent>(m_SceneEntity).World->GetGravity().y;
}

void Scene::SetPhysics2DGravity(float gravity)
{
    m_Registry.get<Box2DWorldComponent>(m_SceneEntity).World->SetGravity({ 0.0f, gravity });
}

Ref<Scene> Scene::GetScene(UUID uuid)
{
    if (g_ActiveScenes.find(uuid) != g_ActiveScenes.end())
        return g_ActiveScenes[uuid];

    return nullptr;
}

template<typename T>
static void CopyComponentFromRegistry(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::unordered_map<UUID, entt::entity>& entityMap)
{
    auto view = srcRegistry.view<T>();
    for (auto entity : view)
    {
        entt::entity dstEntity = entityMap.at(srcRegistry.get<IDComponent>(entity).ID);

        auto& srcComponent = view.get<T>(entity);
        dstRegistry.emplace_or_replace<T>(dstEntity, srcComponent);
    }
}

template<typename T>
static void CopyComponentFromEntityIfExists(entt::entity dstEntity, entt::entity srcEntity, entt::registry& registry)
{
    if (registry.has<T>(srcEntity))
    {
        auto& srcComponent = registry.get<T>(srcEntity);
        registry.emplace_or_replace<T>(dstEntity, srcComponent);
    }
}

Entity Scene::CreateEntity(const std::string& name)
{
    return CreateEntity(UUID(), name);
}

Entity Scene::CreateEntity(UUID uuid, const std::string& name)
{
    auto entityHandle = m_Registry.create();
    auto entity = Entity(entityHandle, this);

    auto& idComponent = entity.AddComponent<IDComponent>();
    idComponent.ID = uuid;

    entity.AddComponent<TransformComponent>();
    if (!name.empty())
        entity.AddComponent<TagComponent>(name);

    AB_CORE_ASSERT(m_EntityIDMap.find(uuid) == m_EntityIDMap.end(), "Entity ID already used!");
    m_EntityIDMap[uuid] = entity;

    return entity;
}

Entity Scene::DuplicateEntity(const Entity& entity)
{
    Entity newEntity = CreateEntity(entity.GetComponent<TagComponent>().Tag);

    CopyComponentFromEntityIfExists<TransformComponent>(newEntity, entity, m_Registry);
    CopyComponentFromEntityIfExists<CameraComponent>(newEntity, entity, m_Registry);
    CopyComponentFromEntityIfExists<MeshComponent>(newEntity, entity, m_Registry);
    CopyComponentFromEntityIfExists<ScriptComponent>(newEntity, entity, m_Registry);
    CopyComponentFromEntityIfExists<RigidBody2DComponent>(newEntity, entity, m_Registry);
    CopyComponentFromEntityIfExists<BoxCollider2DComponent>(newEntity, entity, m_Registry);
    CopyComponentFromEntityIfExists<CircleCollider2DComponent>(newEntity, entity, m_Registry);

    return newEntity;
}

void Scene::DestroyEntity(const Entity& entity)
{
    if (entity.HasComponent<ScriptComponent>())
        ScriptEngine::OnScriptComponentDestroyed(m_SceneID, entity.GetUUID());

    auto rigidBody = entity.GetComponentIfExists<RigidBody2DComponent>();
    if (rigidBody && rigidBody->RuntimeBody)
    {
        b2Body* body = static_cast<b2Body*>(rigidBody->RuntimeBody);
        m_Registry.get<Box2DWorldComponent>(m_SceneEntity).World->DestroyBody(body);
    }

    m_Registry.destroy(entity.m_EntityHandle);
}

void Scene::CopyTo(Ref<Scene>& target)
{
    target->m_Light = m_Light;
    target->m_Environment = m_Environment;
    target->m_Skybox = m_Skybox;
    target->m_SkyboxLOD = m_SkyboxLOD;
    target->m_SkyboxMaterial = m_SkyboxMaterial;

    std::unordered_map<UUID, entt::entity> entityMap;
    auto view = m_Registry.view<IDComponent>();
    for (auto& entity : view)
    {
        auto uuid = view.get<IDComponent>(entity).ID;
        Entity e = target->CreateEntity(uuid, "");
        entityMap[uuid] = e.m_EntityHandle;
    }

    CopyComponentFromRegistry<TagComponent>(target->m_Registry, m_Registry, entityMap);
    CopyComponentFromRegistry<TransformComponent>(target->m_Registry, m_Registry, entityMap);
    CopyComponentFromRegistry<CameraComponent>(target->m_Registry, m_Registry, entityMap);
    CopyComponentFromRegistry<MeshComponent>(target->m_Registry, m_Registry, entityMap);
    CopyComponentFromRegistry<ScriptComponent>(target->m_Registry, m_Registry, entityMap);
    CopyComponentFromRegistry<RigidBody2DComponent>(target->m_Registry, m_Registry, entityMap);
    CopyComponentFromRegistry<BoxCollider2DComponent>(target->m_Registry, m_Registry, entityMap);
    CopyComponentFromRegistry<CircleCollider2DComponent>(target->m_Registry, m_Registry, entityMap);

    const auto& instanceMap = ScriptEngine::GetEntityInstanceMap();
    if (instanceMap.find(target->GetUUID()) != instanceMap.end())
        ScriptEngine::CopyEntityScriptData(target->GetUUID(), m_SceneID);

    target->SetPhysics2DGravity(GetPhysics2DGravity());
}

std::vector<entt::entity> Scene::GetAllEntities()
{
    auto& registry = m_Registry;
    std::vector<entt::entity> entities;
    m_Registry.each([&entities, &registry](auto entity) {
        if (!registry.has<SceneComponent>(entity))
            entities.push_back(entity);
    });

    return entities;
}

Entity Scene::GetMainCameraEntity()
{
    auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view)
    {
        auto& camera = view.get<CameraComponent>(entity);
        if (camera.Primary)
            return { entity, this };
    }
    return {};
}

}
