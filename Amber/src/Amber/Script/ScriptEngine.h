#pragma once

#include "Amber/Core/Buffer.h"
#include "Amber/Core/Time.h"

#include "Amber/Scene/Entity.h"

extern "C"
{
    typedef struct _MonoClassField MonoClassField;
    typedef struct _MonoObject MonoObject;
}

namespace Amber
{

enum class FieldType
{
    None = 0, 
    Bool,
    Int, UInt, 
    Float, Vec2, Vec3, Vec4
};

struct EntityScriptClass;
struct EntityInstance
{
    EntityScriptClass* ScriptClass = nullptr;

    uint32_t Handle = 0;
    Scene* SceneInstance = nullptr;

    MonoObject* GetInstance();
};

struct PublicField
{
    std::string Name;
    FieldType Type;

    PublicField() = default;
    PublicField(const std::string& name, FieldType type);
    PublicField(const PublicField& other) = default;
    PublicField(PublicField&& other);

    void CopyStorageToRuntime();
    bool IsRuntimeAvailable() const;

    template<typename T>
    T GetRuntimeValue() const
    {
        T value;
        GetRuntimeValue_Internal(&value);
        return value;
    }

    template<typename T>
    void SetRuntimeValue(T value)
    {
        SetRuntimeValue_Internal(&value);
    }

    template<typename T>
    T GetStoredValue() const
    {
        T value;
        GetStoredValue_Internal(&value);
        return value;
    }

    template<typename T>
    void SetStoredValue(T value)
    {
        SetStoredValue_Internal(&value);
    }

    void SetStoredValue(void* value)
    {
        SetStoredValue_Internal(value);
    }

    static uint32_t GetFieldSize(FieldType type);

    PublicField& operator=(PublicField&& other);

private:
    EntityInstance* Entity = nullptr;
    MonoClassField* ClassField = nullptr;
    Buffer StoredValue;

    void GetRuntimeValue_Internal(void* outValue) const;
    void SetRuntimeValue_Internal(void* value);
    void GetStoredValue_Internal(void* outValue) const;
    void SetStoredValue_Internal(void* value);

    friend class ScriptEngine;
};

using FieldMap = std::unordered_map<std::string, PublicField>;
using ScriptModuleFieldMap = std::unordered_map<std::string, FieldMap>;

struct EntityInstanceData
{
    EntityInstance Instance;
    ScriptModuleFieldMap ModuleFieldMap;
};

using EntityInstanceMap = std::unordered_map<UUID, std::unordered_map<UUID, EntityInstanceData>>;

class ScriptEngine
{
public:
    static void Init();
    static void Shutdown();

    static void LoadRuntimeAssembly(const std::string& assemblyPath, const Scene& scene);

    static void OnSceneDestruct(UUID sceneID);
    static void OnScriptComponentDestroyed(UUID sceneID, UUID entityID);

    static void OnCreateEntity(const Entity& entity);
    static void OnCreateEntity(UUID sceneID, UUID entityID);
    static void OnDestroyEntity(UUID sceneID, UUID entityID);
    static void OnUpdateEntity(UUID sceneID, UUID entityID, Timestep ts);

    static bool ModuleExists(const std::string& moduleName);
    static void InitScriptEntity(const Entity& entity);
    static void ShutdownScriptEntity(const Entity& entity, const std::string& moduleName);
    static void InstantiateEntityClass(const Entity& entity);

    static void CopyEntityScriptData(UUID dstScene, UUID srcScene);

    static Ref<Scene> GetSceneContext();
    static void SetSceneContext(const Ref<Scene>& scene);

    static EntityInstanceMap& GetEntityInstanceMap();
    static EntityInstanceData& GetEntityInstanceData(UUID sceneID, UUID entityID);

    static void OnImGuiRender();
};

}