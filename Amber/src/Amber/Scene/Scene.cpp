#include "abpch.h"
#include "Scene.h"

#include "Amber/Renderer/SceneRenderer.h"

#include "Amber/Scene/Components.h"
#include "Amber/Scene/Entity.h"
#include "Amber/Scene/SceneCamera.h"

#include "Amber/Script/ScriptEngine.h"

namespace Amber
{

static uint32_t s_SceneCounter = 0;
std::unordered_map<UUID, Scene*> g_ActiveScenes;

void OnMeshConstruct(entt::registry& registry, entt::entity entity)
{
    const Ref<Mesh> mesh = registry.get<MeshComponent>(entity);
    if (!mesh)
        return;

    glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 max = glm::vec3(-std::numeric_limits<float>::max());

    for (auto& submesh : mesh->GetSubmeshes())
    {
        glm::vec3 submeshMin = glm::vec3(submesh.Transform * glm::vec4(submesh.BoundingBox.Min, 1.0f));
        glm::vec3 submeshMax = glm::vec3(submesh.Transform * glm::vec4(submesh.BoundingBox.Max, 1.0f));

        min.x = glm::min(glm::min(submeshMin.x, submeshMax.x), min.x);
        min.y = glm::min(glm::min(submeshMin.y, submeshMax.y), min.y);
        min.z = glm::min(glm::min(submeshMin.z, submeshMax.z), min.z);

        max.x = glm::max(glm::max(submeshMin.x, submeshMax.x), max.x);
        max.y = glm::max(glm::max(submeshMin.y, submeshMax.y), max.y);
        max.z = glm::max(glm::max(submeshMin.z, submeshMax.z), max.z);
    }

    Math::AABB aabb(min, max);
    registry.emplace<BoxColliderComponent>(entity, aabb);
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
    m_Registry.on_construct<MeshComponent>().connect<&OnMeshConstruct>();
    m_Registry.on_construct<ScriptComponent>().connect<&OnScriptConstruct>();

    m_SceneEntity = m_Registry.create();
    m_Registry.emplace<SceneComponent>(m_SceneEntity, m_SceneID);

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
}

void Scene::OnRenderEditor(Timestep ts, const EditorCamera& camera)
{
    m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLOD);

    SceneRenderer::BeginScene(this, { camera, camera.GetViewMatrix() });

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

void Scene::OnRenderRuntime(Timestep ts)
{
    m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLOD);

    Entity cameraEntity = GetMainCameraEntity();
    AB_CORE_ASSERT(cameraEntity, "Scene does not contain cameras!");

    glm::mat4 viewMatrix = glm::inverse(cameraEntity.GetComponent<TransformComponent>().Transform);
    SceneCamera& camera = cameraEntity.GetComponent<CameraComponent>();
    camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);

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

Ref<Scene> Scene::GetScene(UUID uuid)
{
    if (g_ActiveScenes.find(uuid) != g_ActiveScenes.end())
        return g_ActiveScenes[uuid];

    return nullptr;
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

void Scene::DestroyEntity(const Entity& entity)
{
    if (entity.HasComponent<ScriptComponent>())
        ScriptEngine::OnScriptComponentDestroyed(m_SceneID, entity.GetUUID());

    m_Registry.destroy(entity.m_EntityHandle);
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
        registry.emplace_or_replace<T>(dstEntity);
    }
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
    CopyComponentFromRegistry<BoxColliderComponent>(target->m_Registry, m_Registry, entityMap);
    CopyComponentFromRegistry<ScriptComponent>(target->m_Registry, m_Registry, entityMap);

    const auto& instanceMap = ScriptEngine::GetEntityInstanceMap();
    if (instanceMap.find(target->GetUUID()) != instanceMap.end())
        ScriptEngine::CopyEntityScriptData(target->GetUUID(), m_SceneID);
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
    auto& view = m_Registry.view<CameraComponent>();
    for (auto entity : view)
    {
        auto& camera = view.get<CameraComponent>(entity);
        if (camera.Primary)
            return { entity, this };
    }
    return {};
}

}
