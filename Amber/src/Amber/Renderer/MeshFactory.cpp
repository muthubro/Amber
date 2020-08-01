#include "abpch.h"
#include "MeshFactory.h"

#include <assimp/Importer.hpp>

#include "Amber/Math/AABB.h"
#include "Amber/Math/Basics.h"

#include "Amber/Renderer/Renderer.h"

namespace Amber
{

Ref<Mesh> MeshFactory::Plane(float width, float height)
{
    auto mesh = Ref<Mesh>::Create();

    mesh->m_FilePath = "Plane";
    mesh->m_IsAnimated = false;

    StaticVertex bottomLeft;
    bottomLeft.Position = glm::vec3(width * 0.5f, 0.0f, -height * 0.5f);
    bottomLeft.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
    bottomLeft.Tangent = glm::vec3(-1.0f, 0.0f, 0.0f);
    bottomLeft.Binormal = glm::vec3(0.0f, 0.0f, 1.0f);
    bottomLeft.TexCoord = glm::vec2(0.0f, 0.0f);
    mesh->m_StaticVertices.push_back(bottomLeft);

    StaticVertex bottomRight;
    bottomRight.Position = glm::vec3(-width * 0.5f, 0.0f, -height * 0.5f);
    bottomRight.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
    bottomRight.Tangent = glm::vec3(0.0f, 0.0f, 1.0f);
    bottomRight.Binormal = glm::vec3(1.0f, 0.0f, 0.0f);
    bottomRight.TexCoord = glm::vec2(1.0f, 0.0f);
    mesh->m_StaticVertices.push_back(bottomRight);

    StaticVertex topRight;
    topRight.Position = glm::vec3(-width * 0.5f, 0.0f, height * 0.5f);
    topRight.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
    topRight.Tangent = glm::vec3(1.0f, 0.0f, 0.0f);
    topRight.Binormal = glm::vec3(0.0f, 0.0f, -1.0f);
    topRight.TexCoord = glm::vec2(1.0f, 1.0f);
    mesh->m_StaticVertices.push_back(topRight);

    StaticVertex topLeft;
    topLeft.Position = glm::vec3(width * 0.5f, 0.0f, height * 0.5f);
    topLeft.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
    topLeft.Tangent = glm::vec3(0.0f, 0.0f, -1.0f);
    topLeft.Binormal = glm::vec3(-1.0f, 0.0f, 0.0f);
    topLeft.TexCoord = glm::vec2(0.0f, 1.0f);
    mesh->m_StaticVertices.push_back(topLeft);

    mesh->m_VertexArray = VertexArray::Create();

    auto vertexBuffer = VertexBuffer::Create(mesh->m_StaticVertices.data(), mesh->m_StaticVertices.size() * sizeof(StaticVertex));
    vertexBuffer->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoord" },
        { ShaderDataType::Float3, "a_Normal" },
        { ShaderDataType::Float3, "a_Tangent" },
        { ShaderDataType::Float3, "a_Binormal" },
    });
    mesh->m_VertexArray->AddVertexBuffer(vertexBuffer);

    mesh->m_Indices.emplace_back(0, 1, 2);
    mesh->m_Indices.emplace_back(2, 3, 0);
    auto indexBuffer = IndexBuffer::Create(mesh->m_Indices.data(), mesh->m_Indices.size() * sizeof(Index));
    mesh->m_VertexArray->SetIndexBuffer(indexBuffer);

    Submesh& submesh = mesh->m_Submeshes.emplace_back(0, 0, 6, 0);
    submesh.BoundingBox = Math::AABB(bottomLeft.Position, topRight.Position);
    
    mesh->m_TriangleCache[0].emplace_back(bottomLeft.Position, bottomRight.Position, topRight.Position);
    mesh->m_TriangleCache[0].emplace_back(topRight.Position, topLeft.Position, bottomLeft.Position);

    mesh->m_BaseMaterial = Ref<Material>::Create(Renderer::GetShaderLibrary()->Get(ShaderType::UnlitColor));
    Ref<MaterialInstance> material = Ref<MaterialInstance>::Create(mesh->m_BaseMaterial);
    mesh->m_Materials.push_back(material);

    return mesh;
}

Ref<Mesh> MeshFactory::Cube(const glm::vec3& center, float length)
{
    Ref<Mesh> mesh = Ref<Mesh>::Create();

    mesh->m_FilePath = "Cube";
    mesh->m_IsAnimated = false;

    glm::vec3 points[] = {
        glm::normalize(glm::vec3( 1.0f, -1.0f, -1.0f)),
        glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)),
        glm::normalize(glm::vec3(-1.0f,  1.0f, -1.0f)),
        glm::normalize(glm::vec3( 1.0f,  1.0f, -1.0f)),
        glm::normalize(glm::vec3( 1.0f, -1.0f,  1.0f)),
        glm::normalize(glm::vec3(-1.0f, -1.0f,  1.0f)),
        glm::normalize(glm::vec3(-1.0f,  1.0f,  1.0f)),
        glm::normalize(glm::vec3( 1.0f,  1.0f,  1.0f))
    };

    float thetas[] = {
        glm::radians(-135.0f),
        glm::radians(-45.0f),
        glm::radians(45.0f),
        glm::radians(135.0f)
    };

    float phis[] = {
        glm::atan(glm::sqrt(2.0f) * 0.5f) + glm::radians(90.0f),
        glm::atan(glm::sqrt(2.0f) * 0.5f)
    };

    float texCoordY[] = { 0.5f, 0.75f, 0.25f, 1.0f };

    float radius = length * 0.5f * glm::sqrt(3.0f);
    for (uint32_t i = 0; i < 8; i++)
    {
        float theta = thetas[i % 4];
        float phi = phis[(i / 2) % 2];

        StaticVertex vertex;
        vertex.Position = center + points[i] * radius;
        vertex.Normal = points[i];
        vertex.Tangent = glm::vec3(-glm::sin(theta), 0.0f, glm::cos(theta));
        vertex.Binormal = glm::cross(vertex.Normal, vertex.Tangent);
        vertex.TexCoord = glm::vec2(vertex.Position.x > 0.0f ? 0.33f : 0.67f, texCoordY[(i / 2) % 4]);
        mesh->m_StaticVertices.push_back(vertex);
    }

    mesh->m_VertexArray = VertexArray::Create();

    auto vertexBuffer = VertexBuffer::Create(mesh->m_StaticVertices.data(), mesh->m_StaticVertices.size() * sizeof(StaticVertex));
    vertexBuffer->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoord" },
        { ShaderDataType::Float3, "a_Normal" },
        { ShaderDataType::Float3, "a_Tangent" },
        { ShaderDataType::Float3, "a_Binormal" },
    });
    mesh->m_VertexArray->AddVertexBuffer(vertexBuffer);

    mesh->m_Indices.emplace_back(0, 1, 2);
    mesh->m_Indices.emplace_back(2, 3, 0);
    mesh->m_Indices.emplace_back(1, 5, 6);
    mesh->m_Indices.emplace_back(6, 2, 1);
    mesh->m_Indices.emplace_back(5, 4, 7);
    mesh->m_Indices.emplace_back(7, 6, 5);
    mesh->m_Indices.emplace_back(4, 0, 3);
    mesh->m_Indices.emplace_back(3, 7, 4);
    mesh->m_Indices.emplace_back(3, 2, 6);
    mesh->m_Indices.emplace_back(6, 7, 3);
    mesh->m_Indices.emplace_back(4, 5, 1);
    mesh->m_Indices.emplace_back(1, 0, 4);

    auto indexBuffer = IndexBuffer::Create(mesh->m_Indices.data(), mesh->m_Indices.size() * sizeof(Index));
    mesh->m_VertexArray->SetIndexBuffer(indexBuffer);

    Submesh& submesh = mesh->m_Submeshes.emplace_back(0, 0, 36, 0);
    submesh.BoundingBox = Math::AABB(mesh->m_StaticVertices[0].Position, mesh->m_StaticVertices[6].Position);

    for (uint32_t i = 0; i < 12; i++)
        mesh->m_TriangleCache[0].emplace_back(
            mesh->m_StaticVertices[mesh->m_Indices[i].V0].Position, 
            mesh->m_StaticVertices[mesh->m_Indices[i].V1].Position, 
            mesh->m_StaticVertices[mesh->m_Indices[i].V2].Position);

    mesh->m_BaseMaterial = Ref<Material>::Create(Renderer::GetShaderLibrary()->Get(ShaderType::StandardStatic));
    Ref<MaterialInstance> material = Ref<MaterialInstance>::Create(mesh->m_BaseMaterial);
    material->Set("u_Albedo", glm::vec3(1.0f));
    material->Set("u_Roughness", 1.0f);
    material->Set("u_Metalness", 0.0f);
    mesh->m_Materials.push_back(material);

    return mesh;
}

Ref<Mesh> MeshFactory::Sphere(const glm::vec3& center, float radius)
{
    Ref<Mesh> mesh = Ref<Mesh>::Create();

    mesh->m_FilePath = "Sphere";
    mesh->m_IsAnimated = false;

    const uint32_t X_SEGMENTS = 64, Y_SEGMENTS = 64;
    for (uint32_t y = 0; y <= Y_SEGMENTS; y++)
    {
        for (uint32_t x = 0; x <= X_SEGMENTS; x++)
        {
            float xSegment = (float)x / X_SEGMENTS;
            float ySegment = (float)y / Y_SEGMENTS;

            float xPos = (float)(glm::cos(2.0f * Math::PI * xSegment) * glm::sin(Math::PI * ySegment));
            float yPos = (float)glm::cos(Math::PI * ySegment);
            float zPos = (float)(glm::sin(2.0f * Math::PI * xSegment) * glm::sin(Math::PI * ySegment));

            StaticVertex vertex;
            vertex.Position = center + radius * glm::vec3(xPos, yPos, zPos);
            vertex.Normal = glm::normalize(vertex.Position);
            vertex.Tangent = glm::vec3(-glm::sin(2.0f * Math::PI * xSegment), 0.0f, glm::cos(2.0f * Math::PI * xSegment));
            vertex.Binormal = glm::cross(vertex.Normal, vertex.Tangent);
            vertex.TexCoord = glm::vec2(xSegment, ySegment);
            mesh->m_StaticVertices.push_back(vertex);
        }
    }

    mesh->m_VertexArray = VertexArray::Create();

    auto vertexBuffer = VertexBuffer::Create(mesh->m_StaticVertices.data(), mesh->m_StaticVertices.size() * sizeof(StaticVertex));
    vertexBuffer->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoord" },
        { ShaderDataType::Float3, "a_Normal" },
        { ShaderDataType::Float3, "a_Tangent" },
        { ShaderDataType::Float3, "a_Binormal" },
    });
    mesh->m_VertexArray->AddVertexBuffer(vertexBuffer);

    for (int32_t y = Y_SEGMENTS; y > 0; y--)
    {
        for (int32_t x = 0; x <= X_SEGMENTS; x++)
        {
            uint32_t x0 = x, x1 = (x + 1) % (X_SEGMENTS + 1);
            uint32_t y0 = y, y1 = y - 1;
            mesh->m_Indices.emplace_back(
                y0 * (X_SEGMENTS + 1) + x0, 
                y0 * (X_SEGMENTS + 1) + x1, 
                y1 * (X_SEGMENTS + 1) + x1);
            mesh->m_Indices.emplace_back(
                y1 * (X_SEGMENTS + 1) + x1, 
                y1 * (X_SEGMENTS + 1) + x0, 
                y0 * (X_SEGMENTS + 1) + x0);

            mesh->m_TriangleCache[0].emplace_back(
                mesh->m_StaticVertices[y0 * X_SEGMENTS + x0].Position,
                mesh->m_StaticVertices[y0 * X_SEGMENTS + x1].Position,
                mesh->m_StaticVertices[y1 * X_SEGMENTS + x1].Position);
            mesh->m_TriangleCache[0].emplace_back(
                mesh->m_StaticVertices[y1 * X_SEGMENTS + x1].Position,
                mesh->m_StaticVertices[y1 * X_SEGMENTS + x0].Position,
                mesh->m_StaticVertices[y0 * X_SEGMENTS + x0].Position);
        }
    }
    auto indexBuffer = IndexBuffer::Create(mesh->m_Indices.data(), mesh->m_Indices.size() * sizeof(Index));
    mesh->m_VertexArray->SetIndexBuffer(indexBuffer);

    Submesh& submesh = mesh->m_Submeshes.emplace_back(0, 0, (uint32_t)mesh->m_Indices.size() * 3, 0);
    submesh.BoundingBox = Math::AABB(center - glm::vec3(radius), center + glm::vec3(radius));

    mesh->m_BaseMaterial = Ref<Material>::Create(Renderer::GetShaderLibrary()->Get(ShaderType::StandardStatic));
    Ref<MaterialInstance> material = Ref<MaterialInstance>::Create(mesh->m_BaseMaterial);
    material->Set("u_Albedo", glm::vec3(1.0f));
    material->Set("u_Roughness", 1.0f);
    material->Set("u_Metalness", 0.0f);
    mesh->m_Materials.push_back(material);

    return mesh;
}

}