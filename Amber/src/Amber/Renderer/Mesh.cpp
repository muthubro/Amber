#include "abpch.h"
#include "Mesh.h"

#include <filesystem>

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/gtx/quaternion.hpp>

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

#define DEBUG_PRINT_ALL_PROPS 0

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

static std::string GetTexturePath(const std::string& curPath, const std::string& texturePath)
{
    std::filesystem::path path(curPath);
    auto parentPath = path.parent_path();
    parentPath /= std::string(texturePath);
    return parentPath.string();
}

static std::string GetTexturePath(const std::string& curPath, const aiString& texturePath)
{
    return GetTexturePath(curPath, texturePath.data);
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

    m_IsAnimated = m_Scene->HasAnimations();
    m_InverseRootTransform = glm::inverse(AssimpMat4ToMat4(m_Scene->mRootNode->mTransformation));

    uint32_t vertexCount = 0, indexCount = 0;
    uint32_t numMeshes = m_Scene->mNumMeshes;
    m_Submeshes.reserve(numMeshes);
    for (uint32_t m = 0; m < numMeshes; m++)
    {
        aiMesh* mesh = m_Scene->mMeshes[m];
        Submesh& submesh = m_Submeshes.emplace_back(vertexCount, indexCount, mesh->mNumFaces * 3, mesh->mMaterialIndex);
        auto& aabb = submesh.BoundingBox;

        vertexCount += mesh->mNumVertices;
        indexCount += submesh.IndexCount;

        if (m_IsAnimated)
        {
            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
            {
                AnimatedVertex vertex;
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

                m_AnimatedVertices.push_back(vertex);
            }

            for (uint32_t i = 0; i < mesh->mNumBones; i++)
            {
                aiBone* bone = mesh->mBones[i];
                if (bone->mNumWeights == 0)
                    continue;

                std::string boneName = bone->mName.data;
                uint32_t boneIndex = 0;
                if (m_BoneMap.find(boneName) == m_BoneMap.end())
                {
                    BoneInfo boneInfo;
                    boneInfo.Offset = AssimpMat4ToMat4(bone->mOffsetMatrix);

                    m_BoneInfo.push_back(boneInfo);
                    boneIndex = m_BoneCount++;
                    m_BoneMap[boneName] = boneIndex;
                }
                else
                {
                    boneIndex = m_BoneMap[boneName];
                }

                for (uint32_t j = 0; j < bone->mNumWeights; j++)
                {
                    uint32_t vertexID = submesh.BaseVertex + bone->mWeights[j].mVertexId;
                    float weight = bone->mWeights[j].mWeight;

                    auto& aabb = m_BoneInfo[boneIndex].BoundingBox;
                    auto& vertex = m_AnimatedVertices[vertexID];
                    aabb.Min.x = glm::min(vertex.Position.x, aabb.Min.x);
                    aabb.Min.y = glm::min(vertex.Position.y, aabb.Min.y);
                    aabb.Min.z = glm::min(vertex.Position.z, aabb.Min.z);
                    aabb.Max.x = glm::max(vertex.Position.x, aabb.Max.x);
                    aabb.Max.y = glm::max(vertex.Position.y, aabb.Max.y);
                    aabb.Max.z = glm::max(vertex.Position.z, aabb.Max.z);

                    vertex.AddBone(boneIndex, weight);
                }
            }

            m_BoneTransforms.resize(s_MaxBones);
            UpdateBones(0.0f);
        }
        else
        {
            for (uint32_t i = 0; i < mesh->mNumVertices; i++)
            {
                StaticVertex vertex;
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
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; i++)
        {
            AB_CORE_ASSERT(mesh->mFaces[i].mNumIndices == 3, "Faces must have 3 indices!");

            Index index = { mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] };
            m_Indices.push_back(index);

            if (m_IsAnimated)
                m_TriangleCache[m].emplace_back(
                    m_AnimatedVertices[index.V0 + submesh.BaseVertex].Position,
                    m_AnimatedVertices[index.V1 + submesh.BaseVertex].Position,
                    m_AnimatedVertices[index.V2 + submesh.BaseVertex].Position);
            else
                m_TriangleCache[m].emplace_back(
                    m_StaticVertices[index.V0 + submesh.BaseVertex].Position,
                    m_StaticVertices[index.V1 + submesh.BaseVertex].Position,
                    m_StaticVertices[index.V2 + submesh.BaseVertex].Position);
        }
    }

    TraverseNodes(m_Scene->mRootNode);

    m_VertexArray = VertexArray::Create();
    if (m_IsAnimated)
    {
        auto vertexBuffer = VertexBuffer::Create(m_AnimatedVertices.data(), m_AnimatedVertices.size() * sizeof(AnimatedVertex));
        vertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float3, "a_Tangent" },
            { ShaderDataType::Float3, "a_Binormal" },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Int4,   "a_BoneIndices" },
            { ShaderDataType::Float4, "a_BoneWeights" }
        });
        m_VertexArray->AddVertexBuffer(vertexBuffer);
    }
    else
    {
        auto vertexBuffer = VertexBuffer::Create(m_StaticVertices.data(), m_StaticVertices.size() * sizeof(StaticVertex));
        vertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float3, "a_Tangent" },
            { ShaderDataType::Float3, "a_Binormal" },
            { ShaderDataType::Float2, "a_TexCoord" }
        });
        m_VertexArray->AddVertexBuffer(vertexBuffer);
    }

    auto indexBuffer = IndexBuffer::Create(m_Indices.data(), m_Indices.size() * sizeof(Index));
    m_VertexArray->SetIndexBuffer(indexBuffer);

    m_MeshShader = Renderer::GetShaderLibrary()->Get(m_IsAnimated ? "Animated_Lighting" : "Static_Lighting");
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

            auto normalMap = Texture2D::Create(path);
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

            auto roughnessMap = Texture2D::Create(path);
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

        bool metalnessFound = false;
        for (uint32_t i = 0; i < material->mNumProperties; i++)
        {
            auto prop = material->mProperties[i];

#if DEBUG_PRINT_ALL_PROPS
            AB_MESH_LOG("Material Property:");
            AB_MESH_LOG("  Name = {0}", prop->mKey.data);
            // AB_MESH_LOG("  Type = {0}", prop->mType);
            // AB_MESH_LOG("  Size = {0}", prop->mDataLength);
            float data = *(float*)prop->mData;
            AB_MESH_LOG("  Value = {0}", data);

            switch (prop->mSemantic)
            {
                case aiTextureType_NONE:
                    AB_MESH_LOG("  Semantic = aiTextureType_NONE");
                    break;
                case aiTextureType_DIFFUSE:
                    AB_MESH_LOG("  Semantic = aiTextureType_DIFFUSE");
                    break;
                case aiTextureType_SPECULAR:
                    AB_MESH_LOG("  Semantic = aiTextureType_SPECULAR");
                    break;
                case aiTextureType_AMBIENT:
                    AB_MESH_LOG("  Semantic = aiTextureType_AMBIENT");
                    break;
                case aiTextureType_EMISSIVE:
                    AB_MESH_LOG("  Semantic = aiTextureType_EMISSIVE");
                    break;
                case aiTextureType_HEIGHT:
                    AB_MESH_LOG("  Semantic = aiTextureType_HEIGHT");
                    break;
                case aiTextureType_NORMALS:
                    AB_MESH_LOG("  Semantic = aiTextureType_NORMALS");
                    break;
                case aiTextureType_SHININESS:
                    AB_MESH_LOG("  Semantic = aiTextureType_SHININESS");
                    break;
                case aiTextureType_OPACITY:
                    AB_MESH_LOG("  Semantic = aiTextureType_OPACITY");
                    break;
                case aiTextureType_DISPLACEMENT:
                    AB_MESH_LOG("  Semantic = aiTextureType_DISPLACEMENT");
                    break;
                case aiTextureType_LIGHTMAP:
                    AB_MESH_LOG("  Semantic = aiTextureType_LIGHTMAP");
                    break;
                case aiTextureType_REFLECTION:
                    AB_MESH_LOG("  Semantic = aiTextureType_REFLECTION");
                    break;
                case aiTextureType_METALNESS:
                    AB_MESH_LOG("  Semantic = aiTextureType_METALNESS");
                    break;
                case aiTextureType_UNKNOWN:
                    AB_MESH_LOG("  Semantic = aiTextureType_UNKNOWN");
                    AB_MESH_LOG("  Type = {}", prop->mType);
                    break;
            }
#endif
            // TODO: Try to improve this
            if (prop->mType == aiPTI_String && std::string(prop->mKey.data) == "$raw.ReflectionFactor|file")
            {
                uint32_t strLength = *(uint32_t*)prop->mData;
                std::string texturePath(prop->mData + 4, strLength);

                std::string path = GetTexturePath(filepath, texturePath);
                AB_MESH_LOG("    Metalness(reflection factor) map path = {0}", path);

                auto metalnessMap = Texture2D::Create(path);
                if (metalnessMap->Loaded())
                {
                    materialInstance->Set("u_MetalnessTexture", metalnessMap);
                    materialInstance->Set("u_MetalnessTexToggle", 1);
                }
                else
                {
                    AB_CORE_ERROR("Could not load metalness texture {}", path);
                }

                metalnessFound = true;
                break;
            }
        }
        if (!metalnessFound)
        {
            AB_MESH_LOG("    No metalness textures found.");
            materialInstance->Set("u_Metalness", metalness);
        }

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
    if (m_IsAnimated)
    {
        if (m_AnimationPlaying)
        {
            float ticksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f) * m_TimeMultiplier;
            m_AnimationTime += ts * ticksPerSecond;
            m_AnimationTime = fmod(m_AnimationTime, (float)m_Scene->mAnimations[0]->mDuration);
        }

        UpdateBones(m_AnimationTime);
    }
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

void Mesh::UpdateBones(float time)
{
    TraverseNodeHierarchy(m_AnimationTime, m_Scene->mRootNode);

    for (uint32_t m = 0; m < m_Scene->mNumMeshes; m++)
    {
        const aiMesh* mesh = m_Scene->mMeshes[m];
        Submesh& submesh = m_Submeshes[m];
        auto& aabb = submesh.BoundingBox;
        aabb = AABB();

        for (uint32_t i = 0; i < mesh->mNumBones; i++)
        {
            const aiBone* bone = mesh->mBones[i];
            std::string boneName(bone->mName.data);
            if (m_BoneMap.find(boneName) != m_BoneMap.end())
            {
                uint32_t boneIndex = m_BoneMap[boneName];
                const auto& boneInfo = m_BoneInfo[boneIndex];

                glm::vec3 min = boneInfo.FinalTransform * glm::vec4(boneInfo.BoundingBox.Min, 1.0f);
                glm::vec3 max = boneInfo.FinalTransform * glm::vec4(boneInfo.BoundingBox.Max, 1.0f);

                aabb.Min.x = glm::min(min.x, aabb.Min.x);
                aabb.Min.y = glm::min(min.y, aabb.Min.y);
                aabb.Min.z = glm::min(min.z, aabb.Min.z);
                aabb.Max.x = glm::max(max.x, aabb.Max.x);
                aabb.Max.y = glm::max(max.y, aabb.Max.y);
                aabb.Max.z = glm::max(max.z, aabb.Max.z);
            }
        }
    }
}

void Mesh::TraverseNodeHierarchy(float time, const aiNode* node, const glm::mat4& parentTransform)
{
    std::string nodeName(node->mName.data);
    const aiAnimation* animation = m_Scene->mAnimations[0];
    const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);

    glm::mat4 nodeTransform = AssimpMat4ToMat4(node->mTransformation);
    if (nodeAnim)
    {
        glm::vec3 translation = InterpolateTranslation(time, nodeAnim);
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);

        glm::quat rotation = InterpolateRotation(time, nodeAnim);
        glm::mat4 rotationMatrix = glm::toMat4(rotation);

        glm::vec3 scale = InterpolateScale(time, nodeAnim);
        glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), scale);

        nodeTransform = translationMatrix * rotationMatrix * scalingMatrix;
    }

    glm::mat4 transform = parentTransform * nodeTransform;

    if (m_BoneMap.find(nodeName) != m_BoneMap.end())
    {
        uint32_t boneIndex = m_BoneMap[nodeName];
        m_BoneInfo[boneIndex].FinalTransform = m_InverseRootTransform * transform * m_BoneInfo[boneIndex].Offset;
        m_BoneTransforms[boneIndex] = m_BoneInfo[boneIndex].FinalTransform;
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++)
        TraverseNodeHierarchy(time, node->mChildren[i], transform);
}

const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName)
{
    for (uint32_t i = 0; i < animation->mNumChannels; i++)
    {
        const aiNodeAnim* nodeAnim = animation->mChannels[i];
        if (nodeName == nodeAnim->mNodeName.data)
            return nodeAnim;
    }

    return nullptr;
}

uint32_t Mesh::FindPosition(float time, const aiNodeAnim* nodeAnim)
{
    for (uint32_t i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
    {
        if (time < nodeAnim->mPositionKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

uint32_t Mesh::FindRotation(float time, const aiNodeAnim* nodeAnim)
{
    for (uint32_t i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
    {
        if (time < nodeAnim->mRotationKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

uint32_t Mesh::FindScaling(float time, const aiNodeAnim* nodeAnim)
{
    for (uint32_t i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
    {
        if (time < nodeAnim->mScalingKeys[i + 1].mTime)
            return i;
    }

    return 0;
}

glm::vec3 Mesh::InterpolateTranslation(float time, const aiNodeAnim* nodeAnim)
{
    if (nodeAnim->mNumPositionKeys == 1)
    {
        auto value = nodeAnim->mPositionKeys[0].mValue;
        return { value.x, value.y, value.z };
    }

    uint32_t positionIndex = FindPosition(time, nodeAnim);
    uint32_t nextPositionIndex = positionIndex + 1;
    float factor = (time - (float)nodeAnim->mPositionKeys[positionIndex].mTime) / (float)(nodeAnim->mPositionKeys[nextPositionIndex].mTime - nodeAnim->mPositionKeys[positionIndex].mTime);
    if (factor < 0.0f)
        factor = 0.0f;

    const aiVector3D& startPosition = nodeAnim->mPositionKeys[positionIndex].mValue;
    const aiVector3D& endPosition = nodeAnim->mPositionKeys[nextPositionIndex].mValue;
    aiVector3D position = startPosition + (endPosition - startPosition) * factor;
    
    return { position.x, position.y, position.z };
}

glm::quat Mesh::InterpolateRotation(float time, const aiNodeAnim* nodeAnim)
{
    if (nodeAnim->mNumRotationKeys == 1)
    {
        auto value = nodeAnim->mRotationKeys[0].mValue;
        return { value.w, value.x, value.y, value.z };
    }

    uint32_t rotationIndex = FindRotation(time, nodeAnim);
    uint32_t nextRotationIndex = rotationIndex + 1;
    float factor = (time - (float)nodeAnim->mRotationKeys[rotationIndex].mTime) / (float)(nodeAnim->mRotationKeys[nextRotationIndex].mTime - nodeAnim->mRotationKeys[rotationIndex].mTime);
    if (factor < 0.0f)
        factor = 0.0f;

    const aiQuaternion& startRotation = nodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion& endRotation = nodeAnim->mRotationKeys[nextRotationIndex].mValue;
    aiQuaternion rotation;
    aiQuaternion::Interpolate(rotation, startRotation, endRotation, factor);
    rotation.Normalize();

    return { rotation.w, rotation.x, rotation.y, rotation.z };
}

glm::vec3 Mesh::InterpolateScale(float time, const aiNodeAnim* nodeAnim)
{
    if (nodeAnim->mNumScalingKeys == 1)
    {
        auto value = nodeAnim->mScalingKeys[0].mValue;
        return { value.x, value.y, value.z };
    }

    uint32_t scalingIndex = FindScaling(time, nodeAnim);
    uint32_t nextScalingIndex = scalingIndex + 1;
    float factor = (time - (float)nodeAnim->mScalingKeys[scalingIndex].mTime) / (float)(nodeAnim->mScalingKeys[nextScalingIndex].mTime - nodeAnim->mScalingKeys[scalingIndex].mTime);
    if (factor < 0.0f)
        factor = 0.0f;

    const aiVector3D& startScale = nodeAnim->mScalingKeys[scalingIndex].mValue;
    const aiVector3D& endScale = nodeAnim->mScalingKeys[nextScalingIndex].mValue;
    aiVector3D scale = startScale + (endScale - startScale) * factor;

    return { scale.x, scale.y, scale.z };
}

}