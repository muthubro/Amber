#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/Timestep.h"

#include "Amber/Renderer/Material.h"
#include "Amber/Renderer/Shader.h"
#include "Amber/Renderer/Texture.h"
#include "Amber/Renderer/VertexArray.h"

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

struct Submesh
{
    uint32_t BaseVertex;
    uint32_t BaseIndex;
    uint32_t IndexCount;
    uint32_t MaterialIndex;

    glm::mat4 Transform = glm::mat4(1.0f);

    Submesh(uint32_t baseVertex, uint32_t baseIndex, uint32_t indexCount, uint32_t materialIndex)
        : BaseVertex(baseVertex), BaseIndex(baseIndex), IndexCount(indexCount), MaterialIndex(materialIndex) {}
};

class Mesh
{
public:
    Mesh(const std::string& filepath);

    void Bind();
    void OnUpdate(Timestep ts);

    std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
    const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }

    const std::string& GetFilePath() const { return m_FilePath; }
    Ref<Material> GetMaterial() { return m_BaseMaterial; }
    Ref<Shader> GetMeshShader() { return m_MeshShader; }
    const std::vector<Ref<MaterialInstance>>& GetMaterials() const { return m_Materials; }
    const std::vector<Ref<Texture2D>>& GetTextures() const { return m_Textures; }

private:
    std::string m_FilePath;

    Scope<Assimp::Importer> m_Importer;
    const aiScene* m_Scene;

    std::vector<Submesh> m_Submeshes;

    std::vector<Vertex> m_StaticVertices;
    std::vector<Index> m_Indices;
    Ref<VertexArray> m_VertexArray;

    Ref<Material> m_BaseMaterial;
    Ref<Shader> m_MeshShader;
    std::vector<Ref<MaterialInstance>> m_Materials;
    std::vector<Ref<Texture2D>> m_Textures;
    std::vector<Ref<Texture2D>> m_NormalMaps;
};

}