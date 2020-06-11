#pragma once

#include "Amber/Renderer/Camera.h"
#include "Amber/Renderer/Material.h"
#include "Amber/Renderer/Texture.h"

#include "Amber/Scene/Entity.h"

namespace Amber
{

struct Environment
{
    Ref<TextureCube> IrradianceMap;
    Ref<TextureCube> RadianceMap;

    static Environment Load(const std::string& filepath);
};

struct Light
{
    glm::vec3 Direction;
    glm::vec3 Radiance;

    float Multiplier = 1.0f;
};

class Scene : public RefCounted
{
public:
    Scene(const std::string& debugName = "Scene");
    ~Scene();

    void Init();

    void OnUpdate(Timestep ts);
    void OnEvent(Event& e);

    Camera& GetCamera() { return m_Camera; }
    void SetCamera(const Camera& camera) { m_Camera = camera; }

    Light& GetLight() { return m_Light; }
    float GetSkyboxLOD() { return m_SkyboxLOD; }

    const Environment& GetEnvironment() { return m_Environment; }
    void SetEnvironment(const Environment& environment);

    Ref<MaterialInstance> GetSkyboxMaterial() { return m_SkyboxMaterial; }
    void SetSkybox(const Ref<TextureCube>& skybox);

    void AddEntity(Entity* entity);
    Entity* CreateEntity(const std::string& name = "Entity");

private:
    std::string m_DebugName;

    Camera m_Camera;
    std::vector<Entity*> m_Entities;
    Environment m_Environment;
    Light m_Light;

    Ref<TextureCube> m_Skybox;
    Ref<MaterialInstance> m_SkyboxMaterial;
    float m_SkyboxLOD = 0.0f;
};

}
