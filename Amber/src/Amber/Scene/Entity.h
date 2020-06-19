#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/Base.h"

#include "Amber/Core/Math/AABB.h"

#include "Amber/Renderer/Mesh.h"

#include "Amber/Scene/Component.h"
#include "Amber/Scene/Scene.h"

#include <entt/entt.hpp>

namespace Amber
{

class Entity : public RefCounted
{
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene);

    template<typename T, typename... Args>
    T& AddComponent(Args... args)
    {
        return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    bool HasComponent()
    {
        return m_Scene->m_Registry.has<T>(m_EntityHandle);
    }

    template<typename T>
    T& GetComponent()
    {
        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    template<typename T>
    T* GetComponentIfExists()
    {
        return m_Scene->m_Registry.try_get<T>(m_EntityHandle);
    }

    glm::mat4& Transform() { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }
    const glm::mat4& Transform() const { return m_Scene->m_Registry.get<TransformComponent>(m_EntityHandle); }

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
