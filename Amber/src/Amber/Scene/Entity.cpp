#include "abpch.h"
#include "Entity.h"

namespace Amber
{

void OnMeshConstruct(entt::registry& registry, entt::entity entity)
{
    const Ref<Mesh> mesh = registry.get<MeshComponent>(entity);

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
    
    AABB aabb(min, max);
    registry.emplace<BoxCollider>(entity, aabb);
}

Entity::Entity(entt::entity handle, Scene* scene)
    : m_EntityHandle(handle), m_Scene(scene)
{
    m_Scene->m_Registry.on_construct<MeshComponent>().connect<&OnMeshConstruct>();
}

}