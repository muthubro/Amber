#include "abpch.h"
#include "Mesh.h"

#include <filesystem>

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Amber/Renderer/Buffer.h"
#include "Amber/Renderer/Renderer.h"
#include "Amber/Renderer/Texture.h"
#include "Amber/Renderer/VertexArray.h"

namespace Amber
{

#define MESH_DEBUG_LOG 1
#if MESH_DEBUG_LOG
#define AB_MESH_LOG(...) AB_CORE_TRACE(__VA_ARGS__)
#else
#define AB_MESH_LOG(...)
#endif

static const uint32_t s_MeshImportFlags =
    aiProcess_CalcTangentSpace |
    aiProcess_Triangulate |
    aiProcess_GenNormals |
    aiProcess_SplitLargeMeshes |
    aiProcess_ValidateDataStructure |
    aiProcess_SortByPType |
    aiProcess_GenUVCoords |
    aiProcess_OptimizeMeshes;

glm::mat4 AssimpMat4ToMat4(const aiMatrix4x4& matrix)
{
    glm::mat4 result;

    // a - d : Rows; 1 - 4: Columns
    result[0][0] = matrix.a1; result[0][1] = matrix.b1; result[0][2] = matrix.c1; result[0][3] = matrix.d1;
    result[1][0] = matrix.a2; result[1][1] = matrix.b2; result[1][2] = matrix.c2; result[1][3] = matrix.d2;
    result[2][0] = matrix.a3; result[2][1] = matrix.b3; result[2][2] = matrix.c3; result[2][3] = matrix.d3;
    result[3][0] = matrix.a4; result[3][1] = matrix.b4; result[3][2] = matrix.c4; result[3][3] = matrix.d4;

    return result;
}

struct LogStream : public Assimp::LogStream
{
    static void Initialize()
    {
        if (Assimp::DefaultLogger::isNullLogger())
        {
            Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
            Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
        }
    }

    void write(const char* message) override
    {
        AB_CORE_ERROR(message);
    }
};

static std::string GetTexturePath(const std::string& curPath, const aiString& texturePath)
{
    std::filesystem::path path(curPath);
    auto parentPath = path.parent_path();
    parentPath /= std::string(texturePath.data);
    return parentPath.string();
}

Mesh::Mesh(const std::string& filepath)
    : m_FilePath(filepath)
{
    LogStream::Initialize();

    AB_CORE_TRACE("Loading mesh from {}", filepath);

    m_Importer = CreateScope<Assimp::Importer>();
    m_Scene = m_Importer->ReadFile(filepath, s_MeshImportFlags);

    if (!m_Scene || !m_Scene->HasMeshes())
    {
        AB_CORE_ASSERT(false, "Failed to load mesh!");
        return;
    }

    uint32_t vertexCount = 0, indexCount = 0;
    uint32_t numMeshes = m_Scene->mNumMeshes;
    m_Submeshes.reserve(numMeshes);
    for (uint32_t m = 0; m < numMeshes; m++)
    {
        aiMesh* mesh = m_Scene->mMeshes[m];
        Submesh& submesh = m_Submeshes.emplace_back(vertexCount, indexCount, mesh->mNumFaces * 3, mesh->mMaterialIndex);

        vertexCount += mesh->mNumVertices;
        indexCount += submesh.IndexCount;

        auto& aabb = submesh.BoundingBox;
        aabb.Min = glm::vec3(std::numeric_limits<float>::max());
        aabb.Max = glm::vec3(-std::numeric_limits<float>::max());

        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

            aabb.Min.x = glm::min(vertex.Position.x, aabb.Min.x);
            aabb.Min.y = glm::min(vertex.Position.y, aabb.Min.y);
            aabb.Min.z = glm::min(vertex.Position.z, aabb.Min.z);
            aabb.Max.x = glm::max(vertex.Position.x, aabb.Max.x);
            aabb.Max.y = glm::max(vertex.Position.y, aabb.Max.y);
            aabb.Max.z = glm::max(vertex.Position.z, aabb.Max.z);

            if (mesh->HasTangentsAndBitangents())
            {
                vertex.Tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
                vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
            }

            if (mesh->HasTextureCoords(0))
                vertex.TexCoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
            
            m_StaticVertices.push_back(vertex);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++)
        {
            AB_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Faces must have 3 indices!");

            Index index = { mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] };
            m_Indices.push_back(index);
            m_TriangleCache[m].emplace_back(
                m_StaticVertices[index.V0 + submesh.BaseVertex],
                m_StaticVertices[index.V1 + submesh.BaseVertex],
                m_StaticVertices[index.V2 + submesh.BaseVertex]);
        }
    }

    TraverseNodes(m_Scene->mRootNode);

    m_VertexArray = VertexArray::Create();

    auto vertexBuffer = VertexBuffer::Create(m_StaticVertices.data(), m_StaticVertices.size() * sizeof(Vertex));
    vertexBuffer->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float3, "a_Normal" },
        { ShaderDataType::Float3, "a_Tangent" },
        { ShaderDataType::Float3, "a_Binormal" },
        { ShaderDataType::Float2, "a_TexCoord" }
        });
    m_VertexArray->AddVertexBuffer(vertexBuffer);

    auto indexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));
    m_VertexArray->SetIndexBuffer(indexBuffer);

    m_MeshShader = Renderer::GetShaderLibrary()->Get("Static_Lighting");
    m_BaseMaterial = Ref<Material>::Create(m_MeshShader);

    AB_MESH_LOG("------------------ Materials ------------------");

    uint32_t materialCount = m_Scene->mNumMaterials;
    m_Materials.resize(materialCount);
    m_Textures.resize(materialCount);
    for (uint32_t i = 0; i < materialCount; i++)
    {
        aiMaterial* material = m_Scene->mMaterials[i];
        auto materialInstance = Ref<MaterialInstance>::Create(m_BaseMaterial);
        m_Materials[i] = materialInstance;

        AB_MESH_LOG("  {0} (Index = {1})", material->GetName().data, i);

        aiColor3D diffuseColor;
        float shininess, roughness, metalness;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
        material->Get(AI_MATKEY_SHININESS, shininess);
        material->Get(AI_MATKEY_REFLECTIVITY, metalness);
        roughness = 1.0f - sqrt(shininess * 0.01f);

        AB_MESH_LOG("    COLOR     = {0}, {1}, {2}", diffuseColor.r, diffuseColor.g, diffuseColor.b);
        AB_MESH_LOG("    ROUGHNESS = {0}", roughness);
        AB_MESH_LOG("    METALNESS = {0}", metalness);

        uint32_t textureCount = material->GetTextureCount(aiTextureType_DIFFUSE);
        AB_MESH_LOG("    {} diffuse textures found.", textureCount);

        textureCount = material->GetTextureCount(aiTextureType_SPECULAR);
        AB_MESH_LOG("    {} specular textures found.", textureCount);

        aiString texturePath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string path = GetTexturePath(filepath, texturePath);
            AB_MESH_LOG("    Albedo map path = {0}", path);

            auto albedo = Texture2D::Create(path, true);
            if (albedo->Loaded())
            {
                m_Textures[i] = albedo;
                materialInstance->Set("u_AlbedoTexture", albedo);
                materialInstance->Set("u_AlbedoTexToggle", 1);
            }
            else
            {
                AB_CORE_ERROR("Could not load albedo texture {}.", path);
                materialInstance->Set("u_Albedo", glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b));
            }
        }
        else
        {
            AB_MESH_LOG("    No albedo textures found.");

            materialInstance->Set("u_Albedo", glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b));
        }

        if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string path = GetTexturePath(filepath, texturePath);
            AB_MESH_LOG("    Normal map path = {0}", path);

            auto normalMap = Texture2D::Create(path, true);
            if (normalMap->Loaded())
            {
                materialInstance->Set("u_NormalTexture", normalMap);
                materialInstance->Set("u_NormalTexToggle", 1);
            }
            else
            {
                AB_CORE_ERROR("Could not load normal map {}", path);
            }
        }
        else
        {
            AB_MESH_LOG("    No normal textures found.");
        }

        if (material->GetTexture(aiTextureType_SHININESS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string path = GetTexturePath(filepath, texturePath);
            AB_MESH_LOG("    Roughness(shininess) map path = {0}", path);

            auto roughnessMap = Texture2D::Create(path, true);
            if (roughnessMap->Loaded())
            {
                materialInstance->Set("u_RoughnessTexture", roughnessMap);
                materialInstance->Set("u_RoughnessTexToggle", 1);
            }
            else
            {
                AB_CORE_ERROR("Could not load roughness texture {}", path);
            }
        }
        else
        {
            AB_MESH_LOG("    No roughness textures found.");
            materialInstance->Set("u_Roughness", roughness);
        }

        // TODO: Metalness map
        materialInstance->Set("u_Metalness", metalness);

        AB_MESH_LOG("");
    }

    AB_MESH_LOG("-----------------------------------------------");
}

Mesh::~Mesh()
{
}

void Mesh::Bind()
{
    m_VertexArray->Bind();
}

void Mesh::OnUpdate(Timestep ts)
{
}

void Mesh::TraverseNodes(aiNode* node, const glm::mat4& parentTransform)
{
    glm::mat4 transform = parentTransform * AssimpMat4ToMat4(node->mTransformation);
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        Submesh& submesh = m_Submeshes[node->mMeshes[i]];
        submesh.Transform = transform;
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
        TraverseNodes(node->mChildren[i], transform);
}

}