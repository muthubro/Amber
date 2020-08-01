#include "abpch.h"
#include "ScriptEngineRegistry.h"

#include <mono/jit/jit.h>

#include "Amber/Scene/Entity.h"

#include "ScriptWrappers.h"

namespace Amber
{

std::unordered_map<MonoType*, std::function<bool(Entity& entity)>> s_HasComponentFuncs;
std::unordered_map<MonoType*, std::function<void(Entity& entity)>> s_CreateComponentFuncs;

extern MonoImage* g_CoreAssemblyImage;

#define RegisterComponentType(Type) \
    {\
        MonoType* type = mono_reflection_type_from_name("Amber." #Type, g_CoreAssemblyImage);\
        if (type)\
        {\
            uint32_t id = mono_type_get_type(type);\
            s_HasComponentFuncs[type] = [](Entity& entity) { return entity.HasComponent<Type>(); };\
            s_CreateComponentFuncs[type] = [](Entity& entity) { entity.AddComponent<Type>(); };\
        }\
        else\
        {\
            AB_CORE_ASSERT("No C# class found for " #Type "!");\
        }\
    }

static void InitComponentTypes()
{
    RegisterComponentType(TagComponent);
    RegisterComponentType(TransformComponent);
    RegisterComponentType(CameraComponent);
    RegisterComponentType(MeshComponent);
    RegisterComponentType(ScriptComponent);
    RegisterComponentType(RigidBody2DComponent);
    RegisterComponentType(BoxCollider2DComponent);
    RegisterComponentType(CircleCollider2DComponent);
}

void ScriptEngineRegistry::RegisterAll()
{
    InitComponentTypes();

    mono_add_internal_call("Amber.Noise::PerlinNoise_Native", Amber::Script::Amber_Noise_PerlinNoise);

    mono_add_internal_call("Amber.Input::IsKeyPressed_Native", Amber::Script::Amber_Input_IsKeyPressed);

    mono_add_internal_call("Amber.Entity::HasComponent_Native", Amber::Script::Amber_Entity_HasComponent);
    mono_add_internal_call("Amber.Entity::CreateComponent_Native", Amber::Script::Amber_Entity_CreateComponent);

    mono_add_internal_call("Amber.TagComponent::GetTag_Native", Amber::Script::Amber_TagComponent_GetTag);
    mono_add_internal_call("Amber.TagComponent::SetTag_Native", Amber::Script::Amber_TagComponent_SetTag);

    mono_add_internal_call("Amber.TransformComponent::GetTransform_Native", Amber::Script::Amber_TransformComponent_GetTransform);
    mono_add_internal_call("Amber.TransformComponent::SetTransform_Native", Amber::Script::Amber_TransformComponent_SetTransform);

    mono_add_internal_call("Amber.MeshComponent::GetMesh_Native", Amber::Script::Amber_MeshComponent_GetMesh);
    mono_add_internal_call("Amber.MeshComponent::SetMesh_Native", Amber::Script::Amber_MeshComponent_SetMesh);

    mono_add_internal_call("Amber.RigidBody2DComponent::ApplyLinearImpulse_Native", Amber::Script::Amber_RigidBody2DComponent_ApplyLinearImpulse);

    mono_add_internal_call("Amber.Texture2D::Constructor_Filepath_Native", Amber::Script::Amber_Texture2D_Constructor_Filepath);
    mono_add_internal_call("Amber.Texture2D::Constructor_Size_Native", Amber::Script::Amber_Texture2D_Constructor_Size);
    mono_add_internal_call("Amber.Texture2D::Destructor_Native", Amber::Script::Amber_Texture2D_Destructor);
    mono_add_internal_call("Amber.Texture2D::SetData_Native", Amber::Script::Amber_Texture2D_SetData);

    mono_add_internal_call("Amber.Material::Destructor_Native", Amber::Script::Amber_Material_Destructor);
    mono_add_internal_call("Amber.Material::SetFloat_Native", Amber::Script::Amber_Material_SetFloat);
    mono_add_internal_call("Amber.Material::SetTexture_Native", Amber::Script::Amber_Material_SetTexture);

    mono_add_internal_call("Amber.MaterialInstance::Destructor_Native", Amber::Script::Amber_MaterialInstance_Destructor);
    mono_add_internal_call("Amber.MaterialInstance::SetFloat_Native", Amber::Script::Amber_MaterialInstance_SetFloat);
    mono_add_internal_call("Amber.MaterialInstance::SetVector3_Native", Amber::Script::Amber_MaterialInstance_SetVector3);
    mono_add_internal_call("Amber.MaterialInstance::SetTexture_Native", Amber::Script::Amber_MaterialInstance_SetTexture);

    mono_add_internal_call("Amber.Mesh::Constructor_Native", Amber::Script::Amber_Mesh_Constructor);
    mono_add_internal_call("Amber.Mesh::Destructor_Native", Amber::Script::Amber_Mesh_Destructor);
    mono_add_internal_call("Amber.Mesh::GetMaterial_Native", Amber::Script::Amber_Mesh_GetMaterial);
    mono_add_internal_call("Amber.Mesh::GetMaterialByIndex_Native", Amber::Script::Amber_Mesh_GetMaterialByIndex);
    mono_add_internal_call("Amber.Mesh::GetMaterialCount_Native", Amber::Script::Amber_Mesh_GetMaterialCount);
    mono_add_internal_call("Amber.Mesh::GetSubmeshCount_Native", Amber::Script::Amber_Mesh_GetSubmeshCount);
    mono_add_internal_call("Amber.Mesh::SetAlbedoTexture_Native", Amber::Script::Amber_Mesh_SetAlbedoTexture);

    mono_add_internal_call("Amber.MeshFactory::CreatePlane_Native", Amber::Script::Amber_MeshFactory_CreatePlane);
    mono_add_internal_call("Amber.MeshFactory::CreateCube_Native", Amber::Script::Amber_MeshFactory_CreateCube);
    mono_add_internal_call("Amber.MeshFactory::CreateSphere_Native", Amber::Script::Amber_MeshFactory_CreateSphere);
}

}