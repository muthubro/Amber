#pragma once

#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Amber/Core/Time.h"

#include "Amber/Core/Math/AABB.h"

#include "Amber/Renderer/Material.h"
#include "Amber/Renderer/Shader.h"
#include "Amber/Renderer/Texture.h"
#include "Amber/Renderer/VertexArray.h"

struct aiAnimation;
struct aiNode;
struct aiNodeAnim;
struct aiScene;

namespace Assimp
{
class Importer;
}

namespace Amber
{

struct StaticVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Binormal;
    glm::vec2 TexCoord;
};

struct AnimatedVertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Binormal;
    glm::vec2 TexCoord;

    uint32_t BoneID[4]{ 0, 0, 0, 0 };
    float BoneWeight[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

    void AddBone(uint32_t boneIndex, float boneWeight)
    {
        for (uint32_t i = 0; i < 4; i++)
        {
            if (BoneWeight[i] == 0.0f)
            {
                BoneID[i] = boneIndex;
                BoneWeight[i] = boneWeight;
                return;
            }
        }

        AB_CORE_WARN("StaticVertex can only have upto 4 bones. Bone discarded (ID = {0}, Weight = {1}", boneIndex, boneWeight);
    }
};

struct BoneInfo
{
    AABB BoundingBox;
    glm::mat4 FinalTransform = glm::mat4(1.0f);
    glm::mat4 Offset = glm::mat4(1.0f);
};

struct Index
{
    uint32_t V0, V1, V2;
};

struct Triangle
{
    glm::vec3 V0, V1, V2;

    Triangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
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

    uint32_t GetBoneCount() const { return m_BoneCount; }
    const glm::mat4& GetBoneTransform(uint32_t index) const { return m_BoneTransforms[index]; };
    const std::vector<glm::mat4>& GetBoneTransforms() const { return m_BoneTransforms; };

    const std::vector<BoneInfo>& GetBoneInfo() const { return m_BoneInfo; }

    bool IsAnimated() { return m_IsAnimated; }
    void ToggleAnimation() { m_AnimationPlaying = !m_AnimationPlaying; }

private:
    std::string m_FilePath;

    Scope<Assimp::Importer> m_Importer = nullptr;
    const aiScene* m_Scene;

    std::vector<Submesh> m_Submeshes;
    std::unordered_map<uint32_t, std::vector<Triangle>> m_TriangleCache;

    std::vector<AnimatedVertex> m_AnimatedVertices;
    std::vector<StaticVertex> m_StaticVertices;
    std::vector<Index> m_Indices;
    Ref<VertexArray> m_VertexArray;

    glm::mat4 m_InverseRootTransform;

    uint32_t m_BoneCount = 0;
    std::vector<BoneInfo> m_BoneInfo;
    std::unordered_map<std::string, uint32_t> m_BoneMap;
    std::vector<glm::mat4> m_BoneTransforms;

    bool m_AnimationPlaying = true;
    float m_AnimationTime = 0.0f;
    bool m_IsAnimated = false;
    float m_TimeMultiplier = 1.0f;

    Ref<Material> m_BaseMaterial;
    std::vector<Ref<MaterialInstance>> m_Materials;
    Ref<Shader> m_MeshShader;
    std::vector<Ref<Texture2D>> m_Textures;

    inline static const uint32_t s_MaxBones = 100;

    void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f));

    void UpdateBones(float time);
    void TraverseNodeHierarchy(float time, const aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f));
    const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);

    uint32_t FindPosition(float time, const aiNodeAnim* nodeAnim);
    uint32_t FindRotation(float time, const aiNodeAnim* nodeAnim);
    uint32_t FindScaling(float time, const aiNodeAnim* nodeAnim);

    glm::vec3 InterpolateTranslation(float time, const aiNodeAnim* nodeAnim);
    glm::quat InterpolateRotation(float time, const aiNodeAnim* nodeAnim);
    glm::vec3 InterpolateScale(float time, const aiNodeAnim* nodeAnim);
};

}