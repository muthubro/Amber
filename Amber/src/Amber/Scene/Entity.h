#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/Base.h"

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
    void SetMesh(const Ref<Mesh>& mesh) { m_Mesh = mesh; }

private:
    std::string m_Name;
    glm::mat4 m_Transform;

    Ref<MaterialInstance> m_Material;
    Ref<Mesh> m_Mesh;
};

}
