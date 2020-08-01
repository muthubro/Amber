#pragma once

#include "Amber/Core/UUID.h"

#include "Amber/Core/Events/Event.h"

#include "Amber/Editor/EditorCamera.h"

#include "Amber/Renderer/Camera.h"
#include "Amber/Renderer/Material.h"
#include "Amber/Renderer/Mesh.h"
#include "Amber/Renderer/Texture.h"

#include "Amber/Scene/SceneCamera.h"

#include <entt/entt.hpp>

namespace Amber
{

struct Environment
{
    std::string Filepath;;
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

class Entity;
using EntityMap = std::unordered_map<UUID, Entity>;

class Scene : public RefCounted
{
public:
    Scene(const std::string& debugName = "Scene", const std::string& assetPath = "");
    ~Scene();

    void Init();

    void OnEvent(Event& e);
    void OnUpdate(Timestep ts);
    void OnRenderEditor(Timestep ts, const EditorCamera& camera, std::vector<Entity>& selectionContext);
    void OnRenderRuntime(Timestep ts, Entity* sceneCameraEntity = nullptr);

    void OnRuntimeStart();
    void OnRuntimeStop();

    Entity CreateEntity(const std::string& name = "");
    Entity CreateEntity(UUID uuid, const std::string& name = "");
    Entity DuplicateEntity(const Entity& entity);
    void DestroyEntity(const Entity& entity);

    void CopyTo(Ref<Scene>& target);

    template<typename... Types>
    auto GetAllEntitiesWith()
    {
        return m_Registry.view<Types...>();
    }

    std::vector<entt::entity> GetAllEntities();
    Entity GetMainCameraEntity();

    const std::string& GetAssetPath() const { return m_AssetPath; }
    const std::string& GetDebugName() const { return m_DebugName; }

    const Light& GetLight() const { return m_Light; }
    Light& GetLight() { return m_Light; }

    float GetSkyboxLOD() const { return m_SkyboxLOD; }
    float& GetSkyboxLOD() { return m_SkyboxLOD; }

    Environment& GetEnvironment() { return m_Environment; }
    const Environment& GetEnvironment() const { return m_Environment; }

    UUID GetUUID() const { return m_SceneID; }
    const EntityMap& GetEntityMap() const { return m_EntityIDMap; }

    bool IsPlaying() const { return m_IsPlaying; }

    void SetAssetPath(const std::string& assetPath) { m_AssetPath = assetPath; }
    void SetDebugName(const std::string& debugName) { m_DebugName = debugName; }
    
    void SetViewportSize(uint32_t width, uint32_t height) { m_ViewportWidth = width; m_ViewportHeight = height; }
    void SetEnvironment(const Environment& environment);
    void SetLight(const Light& light) { m_Light = light; }
    void SetSkyboxLOD(float lod) { m_SkyboxLOD = lod; }

    Ref<MaterialInstance> GetSkyboxMaterial() { return m_SkyboxMaterial; }
    void SetSkybox(const Ref<TextureCube>& skybox);

    float GetPhysics2DGravity() const;
    void SetPhysics2DGravity(float gravity);

    static Ref<Scene> GetScene(UUID uuid);

private:
    UUID m_SceneID;
    std::string m_AssetPath;
    std::string m_DebugName;
    entt::registry m_Registry;
    entt::entity m_SceneEntity;

    EntityMap m_EntityIDMap;

    uint32_t m_ViewportWidth = 1280;
    uint32_t m_ViewportHeight = 720;

    Environment m_Environment;
    Light m_Light;

    Ref<TextureCube> m_Skybox;
    Ref<MaterialInstance> m_SkyboxMaterial;
    float m_SkyboxLOD = 1.0f;

    bool m_IsPlaying = false;

    friend class Entity;
};

}
