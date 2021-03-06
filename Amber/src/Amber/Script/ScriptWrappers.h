#pragma once

#include "Amber/Core/KeyCodes.h"

#include "Amber/Renderer/Mesh.h"

extern "C"
{
    typedef struct _MonoString MonoString;
    typedef struct _MonoArray MonoArray;
}

namespace Amber
{
namespace Script
{

// Math
float Amber_Noise_PerlinNoise(float x, float y);

// Input
bool Amber_Input_IsKeyPressed(KeyCode key);

// Entity
void Amber_Entity_CreateComponent(uint64_t entityID, void* type);
bool Amber_Entity_HasComponent(uint64_t entityID, void* type);
void Amber_Entity_FindEntitiesByTag(MonoString* tag, MonoArray** outArray);

MonoString* Amber_TagComponent_GetTag(uint64_t entityID);
void Amber_TagComponent_SetTag(uint64_t entityID, const char* inTag);

void Amber_TransformComponent_GetTransform(uint64_t entityID, glm::mat4* outTransform);
void Amber_TransformComponent_SetTransform(uint64_t entityID, glm::mat4* inTransform);

Ref<Mesh>* Amber_MeshComponent_GetMesh(uint64_t entityID);
void Amber_MeshComponent_SetMesh(uint64_t entityID, Ref<Mesh>* inMesh);

void Amber_RigidBody2DComponent_ApplyLinearImpulse(uint64_t entityID, glm::vec2* impulse, glm::vec2* offset, bool wake);
void Amber_RigidBody2DComponent_GetLinearVelocity(uint64_t entityID, glm::vec2* outVelocity);
void Amber_RigidBody2DComponent_SetLinearVelocity(uint64_t entityID, glm::vec2* velocity);

// Renderer
// Texture2D
void* Amber_Texture2D_Constructor_Filepath(MonoString* filepath);
void* Amber_Texture2D_Constructor_Size(uint32_t width, uint32_t height);
void Amber_Texture2D_Destructor(Ref<Texture2D>* _this);
void Amber_Texture2D_SetData(Ref<Texture2D>* _this, MonoArray* inData, int32_t count);

// Material
void Amber_Material_Destructor(Ref<Material>* _this);
void Amber_Material_SetFloat(Ref<Material>* _this, MonoString* uniform, float value);
void Amber_Material_SetTexture(Ref<Material>* _this, MonoString* uniform, Ref<Texture2D>* texture);

void Amber_MaterialInstance_Destructor(Ref<MaterialInstance>* _this);
void Amber_MaterialInstance_SetFloat(Ref<MaterialInstance>* _this, MonoString* uniform, float value);
void Amber_MaterialInstance_SetVector3(Ref<MaterialInstance>* _this, MonoString* uniform, glm::vec3* value);
void Amber_MaterialInstance_SetTexture(Ref<MaterialInstance>* _this, MonoString* uniform, Ref<Texture2D>* texture);

// Mesh
void* Amber_Mesh_Constructor(MonoString* filepath);
void Amber_Mesh_Destructor(Ref<Mesh>* _this);
Ref<Material>* Amber_Mesh_GetMaterial(Ref<Mesh>* _this);
Ref<MaterialInstance>* Amber_Mesh_GetMaterialByIndex(Ref<Mesh>* _this, uint32_t index);
uint32_t Amber_Mesh_GetMaterialCount(Ref<Mesh>* _this);
uint32_t Amber_Mesh_GetSubmeshCount(Ref<Mesh>* _this);
void Amber_Mesh_SetAlbedo(Ref<Mesh>* _this, uint32_t submeshIndex, glm::vec4* color);
void Amber_Mesh_SetAlbedoTexture(Ref<Mesh>* _this, uint32_t submeshIndex, bool use, Ref<Texture2D>* albedo);

// Mesh Factory
void* Amber_MeshFactory_CreatePlane(float width, float height);
void* Amber_MeshFactory_CreateCube(glm::vec3* center, float length);
void* Amber_MeshFactory_CreateSphere(glm::vec3* center, float radius);

} // Script
} // Amber