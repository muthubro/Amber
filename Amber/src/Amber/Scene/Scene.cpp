#include "abpch.h"
#include "Scene.h"

#include "Amber/Renderer/SceneRenderer.h"

#include "Amber/Scene/Components.h"

namespace Amber
{

static uint32_t s_SceneCounter = 0;
std::unordered_map<uint32_t, Scene*> g_ActiveScenes;

Environment Environment::Load(const std::string& filepath)
{
    auto [irradiance, radiance] = SceneRenderer::CreateEnvironmentMap(filepath);
    return { irradiance, radiance };
}

Scene::Scene(const std::string& debugName)
    : m_DebugName(debugName), m_SceneID(s_SceneCounter++)
{
    m_SceneEntity = m_Registry.create();
    m_Registry.emplace<SceneComponent>(m_SceneEntity);

    g_ActiveScenes[m_SceneID] = this;

    Init();
}

Scene::~Scene()
{
    m_Registry.clear();
    g_ActiveScenes.erase(m_SceneID);
}

void Scene::Init()
{
    auto skyboxShader = Shader::Create("assets/shaders/Skybox.glsl");
    m_SkyboxMaterial = Ref<MaterialInstance>::Create(Ref<Material>::Create(skyboxShader));
    m_SkyboxMaterial->SetFlag(MaterialFlag::DepthTest, false);
}

void Scene::OnUpdate(Timestep ts)
{
    m_SkyboxMaterial->Set("u_TextureLod", m_SkyboxLOD);

    Camera* camera = nullptr;
    auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view)
    {
        auto& component = view.get<CameraComponent>(entity);
        camera = &component.Camera;
        break;
    }
    AB_CORE_ASSERT(camera, "Scene does not contain a camera!");

    camera->OnUpdate(ts);

    SceneRenderer::BeginScene(this, *camera);

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

void Scene::OnEvent(Event& e)
{
    auto view = m_Registry.view<CameraComponent>();
    for (auto entity : view)
    {
        Camera& camera = view.get<CameraComponent>(entity);
        camera.OnEvent(e);
        break;
    }
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

Entity Scene::CreateEntity(const std::string& name)
{
    auto entityHandle = m_Registry.create();
    auto entity = Entity(entityHandle, this);
    entity.AddComponent<TransformComponent>();
    if (!name.empty())
        entity.AddComponent<TagComponent>(name);

    return entity;
}

void Scene::DeleteEntity(const Entity& entity)
{
    m_Registry.destroy(entity.m_EntityHandle);
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

}
