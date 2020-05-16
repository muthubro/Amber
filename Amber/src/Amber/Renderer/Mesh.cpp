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

static const uint32_t s_MeshImportFlags =
    aiProcess_CalcTangentSpace |
    aiProcess_Triangulate |
    aiProcess_GenNormals |
    aiProcess_SplitLargeMeshes |
    aiProcess_ValidateDataStructure |
    aiProcess_SortByPType |
    aiProcess_GenUVCoords |
    aiProcess_OptimizeMeshes;

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
    parentPath /= (const std::string&)texturePath;
    return parentPath.string();
}

Mesh::Mesh(const std::string& filepath)
    : m_FilePath(filepath)
{
    LogStream::Initialize();

    AB_CORE_TRACE("Loading mesh from {}", filepath);

    m_Importer = CreateScope<Assimp::Importer>();
    m_Scene = m_Importer->ReadFile(filepath, s_MeshImportFlags);

    if (!m_Scene || m_Scene->HasMeshes())
        AB_CORE_ASSERT(false, "Failed to load mesh!");

    uint32_t vertexCount = 0, indexCount = 0;
    uint32_t numMeshes = m_Scene->mNumMeshes;
    m_Submeshes.reserve(numMeshes);
    for (uint32_t i = 0; i < numMeshes; i++)
    {
        aiMesh* mesh = m_Scene->mMeshes[i];
        m_Submeshes.emplace_back(vertexCount, indexCount, mesh->mNumFaces * 3, mesh->mMaterialIndex);

        vertexCount += mesh->mNumVertices;
        indexCount += mesh->mNumFaces * 3;

        for (uint32_t i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
            vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

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
        }

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
    }

    m_MeshShader = Renderer::GetShaderLibrary()->Get("MeshShader");
    m_BaseMaterial = CreateRef<Material>(m_MeshShader);

    uint32_t materialCount = m_Scene->mNumMaterials;
    m_Materials.resize(materialCount);
    m_Textures.resize(materialCount);
    for (uint32_t i = 0; i < materialCount; i++)
    {
        aiMaterial* material = m_Scene->mMaterials[i];
        m_Materials[i] = CreateRef<MaterialInstance>(m_BaseMaterial);

        aiColor3D diffuseColor;
        float shininess, roughness, metalness;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
        material->Get(AI_MATKEY_SHININESS, shininess);
        material->Get(AI_MATKEY_REFLECTIVITY, metalness);
        roughness = 1.0f - sqrt(shininess * 0.01f);

        aiString texturePath;

        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
        {
            std::string path = GetTexturePath(filepath, texturePath);
            auto albedo = Texture2D::Create(path, true);

            if (albedo->Loaded())
            {
                m_Textures[i] = albedo;
            }
            else
            {
                AB_CORE_ERROR("Could not load albedo texture {}", path);
            }
        }
        else
        {
        }

        if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string path = GetTexturePath(filepath, texturePath);
            auto normal = Texture2D::Create(path, true);

            if (normal->Loaded())
            {
            }
            else
            {
                AB_CORE_ERROR("Could not load albedo texture {}", path);
            }
        }
        else
        {
        }

        if (material->GetTexture(aiTextureType_SHININESS, 0, &texturePath) == AI_SUCCESS)
        {
            std::string path = GetTexturePath(filepath, texturePath);
            auto roughness = Texture2D::Create(path, true);

            if (roughness->Loaded())
            {
            }
            else
            {
                AB_CORE_ERROR("Could not load albedo texture {}", path);
            }
        }
        else
        {
        }

        // TODO: Metalness map
    }
}

void Mesh::OnUpdate(Timestep ts)
{
}

}