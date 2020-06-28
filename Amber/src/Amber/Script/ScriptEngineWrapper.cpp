#include "abpch.h"
#include "ScriptEngineWrapper.h"

#include <glm/gtc/type_ptr.hpp>

#include <mono/jit/jit.h>

#include "Amber/Core/Input.h"

#include "Amber/Math/Noise.h"

#include "Amber/Scene/Scene.h"
#include "Amber/Scene/Entity.h"

namespace Amber
{

extern MonoDomain* g_MonoDomain;
extern std::unordered_map<uint32_t, Scene*> g_ActiveScenes;
extern std::unordered_map<MonoType*, std::function<bool(Entity& entity)>> s_HasComponentFuncs;
extern std::unordered_map<MonoType*, std::function<void(Entity& entity)>> s_CreateComponentFuncs;

namespace Script
{

//////////////////////////////////////////////////////////////////////////////////
////////// Math //////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

float Amber_Noise_PerlinNoise(float x, float y)
{
    return Math::Noise::PerlinNoise(x, y);
}

//////////////////////////////////////////////////////////////////////////////////
////////// Input /////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

bool Amber_Input_IsKeyPressed(KeyCode key)
{
    return Input::IsKeyPressed(key);
}

//////////////////////////////////////////////////////////////////////////////////
////////// Entity ////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void Amber_Entity_CreateComponent(uint32_t sceneID, uint32_t entityID, void* type)
{
    AB_CORE_ASSERT(g_ActiveScenes.find(sceneID) != g_ActiveScenes.end(), "Invalid Scene ID!");

    MonoType* component = mono_reflection_type_get_type((MonoReflectionType*)type);
    Scene* scene = g_ActiveScenes[sceneID];
    Entity entity((entt::entity)entityID, scene);
    s_CreateComponentFuncs[component](entity);
}

bool Amber_Entity_HasComponent(uint32_t sceneID, uint32_t entityID, void* type)
{
    AB_CORE_ASSERT(g_ActiveScenes.find(sceneID) != g_ActiveScenes.end(), "Invalid Scene ID!");

    MonoType* component = mono_reflection_type_get_type((MonoReflectionType*)type);
    Scene* scene = g_ActiveScenes[sceneID];
    Entity entity((entt::entity)entityID, scene);
    return s_HasComponentFuncs[component](entity);
}

MonoString* Amber_TagComponent_GetTag(uint32_t sceneID, uint32_t entityID)
{
    AB_CORE_ASSERT(g_ActiveScenes.find(sceneID) != g_ActiveScenes.end(), "Invalid Scene ID!");

    Scene* scene = g_ActiveScenes[sceneID];
    Entity entity((entt::entity)entityID, scene);
    auto& tag = entity.GetComponent<TagComponent>();
    return mono_string_new(g_MonoDomain, tag.Tag.c_str());
}

void Amber_TagComponent_SetTag(uint32_t sceneID, uint32_t entityID, const char* inTag)
{
    AB_CORE_ASSERT(g_ActiveScenes.find(sceneID) != g_ActiveScenes.end(), "Invalid Scene ID!");

    Scene* scene = g_ActiveScenes[sceneID];
    Entity entity((entt::entity)entityID, scene);
    auto& tag = entity.GetComponent<TagComponent>();
    tag.Tag = inTag;
}

void Amber_TransformComponent_GetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4* outTransform)
{
    AB_CORE_ASSERT(g_ActiveScenes.find(sceneID) != g_ActiveScenes.end(), "Invalid Scene ID!");

    Scene* scene = g_ActiveScenes[sceneID];
    Entity entity((entt::entity)entityID, scene);
    auto& transform = entity.GetComponent<TransformComponent>();
    memcpy(outTransform, glm::value_ptr(transform.Transform), sizeof(glm::mat4));
}

void Amber_TransformComponent_SetTransform(uint32_t sceneID, uint32_t entityID, glm::mat4* inTransform)
{
    AB_CORE_ASSERT(g_ActiveScenes.find(sceneID) != g_ActiveScenes.end(), "Invalid Scene ID!");

    Scene* scene = g_ActiveScenes[sceneID];
    Entity entity((entt::entity)entityID, scene);
    auto& transform = entity.GetComponent<TransformComponent>();
    memcpy(glm::value_ptr(transform.Transform), inTransform, sizeof(glm::mat4));
}

Ref<Mesh>* Amber_MeshComponent_GetMesh(uint32_t sceneID, uint32_t entityID)
{
    AB_CORE_ASSERT(g_ActiveScenes.find(sceneID) != g_ActiveScenes.end(), "Invalid Scene ID!");

    Scene* scene = g_ActiveScenes[sceneID];
    Entity entity((entt::entity)entityID, scene);
    auto& mesh = entity.GetComponent<MeshComponent>();
    return &mesh.Mesh;
}

void Amber_MeshComponent_SetMesh(uint32_t sceneID, uint32_t entityID, Ref<Mesh>* inMesh)
{
    AB_CORE_ASSERT(g_ActiveScenes.find(sceneID) != g_ActiveScenes.end(), "Invalid Scene ID!");

    Scene* scene = g_ActiveScenes[sceneID];
    Entity entity((entt::entity)entityID, scene);
    auto& mesh = entity.GetComponent<MeshComponent>();
    mesh.Mesh = inMesh ? *inMesh : nullptr;
}

//////////////////////////////////////////////////////////////////////////////////
////////// Renderer //////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

// Texture2D
void* Amber_Texture2D_Constructor(uint32_t width, uint32_t height)
{
    return new Ref<Texture2D>(Texture2D::Create(TextureFormat::RGBA, width, height));
}

void Amber_Texture2D_Destructor(Ref<Texture2D>* _this)
{
    delete _this;
}

void Amber_Texture2D_SetData(Ref<Texture2D>* _this, MonoArray* inData, int32_t count)
{
    auto& instance = *_this;
    uint32_t dataSize = count * sizeof(float);

    instance->Lock();
    auto& buffer = instance->GetWritableBuffer();
    
    AB_CORE_ASSERT(dataSize <= buffer.Size, "Too much data!");

    size_t pixelSize = sizeof(uint32_t);
    size_t textureSize = (size_t)instance->GetWidth() * instance->GetHeight();
    size_t offset = 0;
    for (size_t i = 0; i < textureSize; i++)
    {
        glm::vec4& value = mono_array_get(inData, glm::vec4, i);
        
        byte color[4];
        color[0] = (uint32_t)(value.r * 255.0f);
        color[1] = (uint32_t)(value.g * 255.0f);
        color[2] = (uint32_t)(value.b * 255.0f);
        color[3] = (uint32_t)(value.a * 255.0f);
        buffer.Write(color, pixelSize, offset);
        offset += pixelSize;
    }

    instance->Unlock();
}

// Material
void Amber_Material_Destructor(Ref<Material>* _this)
{
    delete _this;
}

void Amber_Material_SetFloat(Ref<Material>* _this, MonoString* uniform, float value)
{
    auto& material = *_this;
    material->Set(mono_string_to_utf8(uniform), value);
}

void Amber_Material_SetTexture(Ref<Material>* _this, MonoString* uniform, Ref<Texture2D>* texture)
{
    auto& material = *_this;
    material->Set(mono_string_to_utf8(uniform), *texture);
}

void Amber_MaterialInstance_Destructor(Ref<MaterialInstance>* _this)
{
    delete _this;
}

void Amber_MaterialInstance_SetFloat(Ref<MaterialInstance>* _this, MonoString* uniform, float value)
{
    auto& material = *_this;
    material->Set(mono_string_to_utf8(uniform), value);
}

void Amber_MaterialInstance_SetVector3(Ref<MaterialInstance>* _this, MonoString* uniform, glm::vec3* value)
{
    auto& material = *_this;
    material->Set(mono_string_to_utf8(uniform), *value);
}

void Amber_MaterialInstance_SetTexture(Ref<MaterialInstance>* _this, MonoString* uniform, Ref<Texture2D>* texture)
{
    auto& material = *_this;
    material->Set(mono_string_to_utf8(uniform), *texture);
}

// Mesh
void* Amber_Mesh_Constructor(MonoString* filepath)
{
    return new Ref<Mesh>(Ref<Mesh>::Create(mono_string_to_utf8(filepath)));
}

void Amber_Mesh_Destructor(Ref<Mesh>* _this)
{
    delete _this;
}

Ref<Material>* Amber_Mesh_GetMaterial(Ref<Mesh>* _this)
{
    auto& mesh = *_this;
    return new Ref<Material>(mesh->GetMaterial());
}

Ref<MaterialInstance>* Amber_Mesh_GetMaterialByIndex(Ref<Mesh>* _this, uint32_t index)
{
    auto& mesh = *_this;
    const auto& materials = mesh->GetMaterials();

    AB_CORE_ASSERT(index < materials.size());

    return new Ref<MaterialInstance>(materials[index]);
}

uint32_t Amber_Mesh_GetMaterialCount(Ref<Mesh>* _this)
{
    auto& mesh = *_this;
    return (uint32_t)mesh->GetMaterials().size();
}

uint32_t Amber_Mesh_GetSubmeshCount(Ref<Mesh>* _this)
{
    return (uint32_t)((*_this)->GetSubmeshes().size());
}

void Amber_Mesh_SetAlbedoTexture(Ref<Mesh>* _this, uint32_t submeshIndex, bool use, Ref<Texture2D>* albedo)
{
    auto& mesh = *_this;
    auto& submeshes = mesh->GetSubmeshes();

    AB_CORE_ASSERT(submeshIndex < submeshes.size(), "Submesh index out of bounds!");

    mesh->SetAlbedoTexture(submeshes[submeshIndex], use, *albedo);
}

// Mesh Factory
void* Amber_MeshFactory_CreatePlane(uint32_t width, uint32_t height)
{
    return new Ref<Mesh>(Ref<Mesh>::Create("assets/meshes/Plane1m.obj"));
}

} // Script
} // Amber