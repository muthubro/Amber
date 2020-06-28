#pragma once

#include "Amber/Core/Time.h"

#include "Amber/Scene/Entity.h"

extern "C"
{
    typedef struct _MonoClassField MonoClassField;
}

namespace Amber
{

enum class FieldType
{
    None = 0, 
    Bool,
    Int, UInt, 
    Float, Vec2, Vec3, Vec4,
    String
};

struct EntityInstance;

struct PublicField
{
    std::string Name;
    FieldType Type;

    PublicField(const std::string& name, FieldType type)
        : Name(name), Type(type) {}

    template<typename T>
    T GetValue() const
    {
        T value;
        GetValue_Internal(&value);
        return value;
    }

    template<typename T>
    void SetValue(T value) const
    {
        SetValue_Internal(&value);
    }

private:
    EntityInstance* Entity = nullptr;
    MonoClassField* ClassField = nullptr;

    void GetValue_Internal(void* outValue) const;
    void SetValue_Internal(void* value) const;

    friend class ScriptEngine;
};

using ScriptModuleFieldMap = std::unordered_map<std::string, std::vector<PublicField>>;

class ScriptEngine
{
public:
    static void Init();
    static void Shutdown();

    static void LoadRuntimeAssembly(const std::string& assemblyPath);

    static void OnCreateEntity(const Entity& entity);
    static void OnUpdateEntity(uint32_t entityID, Timestep ts);

    static void OnInitEntity(ScriptComponent& script, uint32_t entityID, uint32_t sceneID);

    static const ScriptModuleFieldMap& GetFieldMap();
};

}