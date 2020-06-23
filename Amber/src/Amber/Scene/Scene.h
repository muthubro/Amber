#pragma once

#include "Amber/Renderer/Camera.h"
#include "Amber/Renderer/Material.h"
#include "Amber/Renderer/Texture.h"

#include <entt/entt.hpp>

namespace Amber
{

class Entity;

struct Environment
{
    Ref<TextureCube> IrradianceMap;
    Ref<TextureCube> RadianceMap;
    float Rotation = 0.0f;

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

    Entity CreateEntity(const std::string& name = "");
    void DeleteEntity(const Entity& entity);

    template<typename... Types>
    auto GetAllEntitiesWith()
    {
        return m_Registry.view<Types...>();
    }

    std::vector<Entity> GetAllEntities();

    Entity* GetEntity(const entt::entity handle) { return m_EntityMap.find(handle) == m_EntityMap.end() ? nullptr : m_EntityMap[handle].Raw(); }

    Light& GetLight() { return m_Light; }
    float GetSkyboxLOD() const { return m_SkyboxLOD; }
    float& GetSkyboxLOD() { return m_SkyboxLOD; }

    Environment& GetEnvironment() { return m_Environment; }
    const Environment& GetEnvironment() const { return m_Environment; }
    
    void SetEnvironment(const Environment& environment);

    Ref<MaterialInstance> GetSkyboxMaterial() { return m_SkyboxMaterial; }
    void SetSkybox(const Ref<TextureCube>& skybox);

private:
    std::string m_DebugName;
    entt::registry m_Registry;
    entt::entity m_SceneEntity;
    uint32_t m_SceneID;

    std::unordered_map<entt::entity, Ref<Entity>> m_EntityMap;

    Environment m_Environment;
    Light m_Light;

    Ref<TextureCube> m_Skybox;
    Ref<MaterialInstance> m_SkyboxMaterial;
    float m_SkyboxLOD = 1.0f;

    friend class Entity;
};

}
