#pragma once

#include <unordered_map>

#include <glm/glm.hpp>

#include "Amber/Core/Time.h"

#include "Amber/Core/Math/AABB.h"

#include "Amber/Renderer/Material.h"
#include "Amber/Renderer/Shader.h"
#include "Amber/Renderer/Texture.h"
#include "Amber/Renderer/VertexArray.h"

struct aiNode;
struct aiScene;

namespace Assimp
{
class Importer;
}

namespace Amber
{

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Binormal;
    glm::vec2 TexCoord;
};

struct Index
{
    uint32_t V0, V1, V2;
};

struct Triangle
{
    Vertex V0, V1, V2;

    Triangle(Vertex v0, Vertex v1, Vertex v2)
        : V0(v0), V1(v1), V2(v2) {}
};

struct Submesh : public RefCounted
{
    uint32_t BaseVertex;
    uint32_t BaseIndex;
    uint32_t IndexCount;
    uint32_t MaterialIndex;

    glm::mat4 Transform = glm::mat4(1.0f);
    AABB BoundingBox;

    Submesh(uint32_t baseVertex, uint32_t baseIndex, uint32_t indexCount, uint32_t materialIndex)
        : BaseVertex(baseVertex), BaseIndex(baseIndex), IndexCount(indexCount), MaterialIndex(materialIndex) {}
};

class Mesh : public RefCounted
{
public:
    Mesh(const std::string& filepath);
    ~Mesh();

    void Bind();
    void OnUpdate(Timestep ts);

    std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
    const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }
    const std::vector<Triangle>& GetTriangleCache(uint32_t index) { return m_TriangleCache[index]; }

    const std::string& GetFilePath() const { return m_FilePath; }
    Ref<Material> GetMaterial() { return m_BaseMaterial; }
    Ref<Shader> GetMeshShader() { return m_MeshShader; }
    const std::vector<Ref<MaterialInstance>>& GetMaterials() const { return m_Materials; }
    const std::vector<Ref<Texture2D>>& GetTextures() const { return m_Textures; }

private:
    std::string m_FilePath;

    Scope<Assimp::Importer> m_Importer = nullptr;
    const aiScene* m_Scene;

    std::vector<Submesh> m_Submeshes;
    std::unordered_map<uint32_t, std::vector<Triangle>> m_TriangleCache;

    std::vector<Vertex> m_StaticVertices;
    std::vector<Index> m_Indices;
    Ref<VertexArray> m_VertexArray;

    Ref<Material> m_BaseMaterial;
    Ref<Shader> m_MeshShader;
    std::vector<Ref<MaterialInstance>> m_Materials;
    std::vector<Ref<Texture2D>> m_Textures;
    std::vector<Ref<Texture2D>> m_NormalMaps;

    void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f));
};

}