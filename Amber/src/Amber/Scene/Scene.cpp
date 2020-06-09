#include "abpch.h"
#include "Scene.h"

#include "Amber/Renderer/SceneRenderer.h"

namespace Amber
{

Environment Environment::Load(const std::string& filepath)
{
    auto [irradiance, radiance] = SceneRenderer::CreateEnvironmentMap(filepath);
    return { irradiance, radiance };
}

Scene::Scene(const std::string& debugName)
    : m_DebugName(debugName)
{
    Init();
}

Scene::~Scene()
{
    for (auto entity : m_Entities)
        delete entity;
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

    for (auto entity : m_Entities)
    {
        auto mesh = entity->GetMesh();
        if (mesh)
            mesh->OnUpdate(ts);
    }

    SceneRenderer::BeginScene(this);

    for (auto entity : m_Entities)
        SceneRenderer::SubmitEntity(entity);

    SceneRenderer::EndScene();
}

void Scene::OnEvent(Event& e)
{
    m_Camera.OnEvent(e);
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

void Scene::AddEntity(Entity* entity)
{
    m_Entities.push_back(entity);
}

Entity* Scene::CreateEntity(const std::string& name)
{
    auto entity = new Entity(name);
    AddEntity(entity);
    return entity;
}

}
