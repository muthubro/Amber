#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/Base.h"

#include "Amber/Core/Math/AABB.h"

#include "Amber/Renderer/Material.h"
#include "Amber/Renderer/Mesh.h"

namespace Amber
{

class Entity : public RefCounted
{
public:
    Entity(const std::string& name) : m_Name(name), m_Transform(1.0f) {}

    const std::string& GetName() const { return m_Name; }

    const glm::mat4& GetTransform() const { return m_Transform; }
    glm::mat4& Transform() { return m_Transform; }

    Ref<MaterialInstance> GetMaterial() { return m_Material; }
    void SetMaterial(const Ref<MaterialInstance>& material) { m_Material = material; }

    Ref<Mesh> GetMesh() { return m_Mesh; }
    void SetMesh(const Ref<Mesh>& mesh) 
    { 
        m_Mesh = mesh; 

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

        m_BoundingBox.Min = min;
        m_BoundingBox.Max = max;
    }

    const AABB& GetBoundingBox() const { return m_BoundingBox; }

private:
    std::string m_Name;
    glm::mat4 m_Transform;
    AABB m_BoundingBox;

    Ref<MaterialInstance> m_Material;
    Ref<Mesh> m_Mesh;
};

}
