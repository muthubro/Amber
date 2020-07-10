#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/Base.h"

#include "Amber/Math/AABB.h"

#include "Amber/Renderer/Mesh.h"

#include "Amber/Scene/Components.h"
#include "Amber/Scene/Scene.h"

#include <entt/entt.hpp>

namespace Amber
{

class Entity
{
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {}

    template<typename T, typename... Args>
    T& AddComponent(Args... args)
    {
        return m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    void RemoveComponent()
    {
        m_Scene->m_Registry.remove_if_exists<T>(m_EntityHandle);
    }

    template<typename T>
    bool HasComponent() const
    {
        return m_Scene->m_Registry.has<T>(m_EntityHandle);
    }

    template<typename T>
    T& GetComponent()
    {
        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    template<typename T>
    const T& GetComponent() const
    {
        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    template<typename T>
    T* GetComponentIfExists()
    {
        return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
    }

    template<typename T>
    const T* GetComponentIfExists() const
    {
        return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
    }

    glm::mat4& Transform() { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }
    const glm::mat4& GetTransform() const { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }

    UUID GetUUID() const { return GetComponent<IDComponent>(); }
    UUID GetSceneUUID() const { return m_Scene->GetUUID(); }

    operator entt::entity() const { return m_EntityHandle; }
    operator uint32_t() const { return (uint32_t)m_EntityHandle; }
    operator bool() const { return (uint32_t)m_EntityHandle && m_Scene; }

    bool operator==(const Entity& other) { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
    bool operator!=(const Entity& other) { return !(*this == other); }

private:
    entt::entity m_EntityHandle;
    Scene* m_Scene = nullptr;

    friend class Scene;
};

}
