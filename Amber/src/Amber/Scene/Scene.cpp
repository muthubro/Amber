#include "abpch.h"
#include "Scene.h"

namespace Amber
{

Environment Environment::Load(const std::string& filepath)
{
    // TODO: Load environment using SceneRenderer
    return Environment();
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
    // TODO: Set skybox shader and material
}

void Scene::OnUpdate(Timestep ts)
{
    m_SkyboxMaterial->Set("u_TextureLOD", m_SkyboxLOD);

    for (auto entity : m_Entities)
    {
        auto mesh = entity->GetMesh();
        if (mesh)
            mesh->OnUpdate(ts);
    }

    // TODO: Submit for rendering
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
