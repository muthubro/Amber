#include "abpch.h"
#include "ScriptEngine.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>

#include "Amber/Core/FileSystem.h"

#include "ScriptEngineRegistry.h"

namespace Amber
{

static void* GetMethodThunk(MonoImage* image, const std::string& methodDesc);

struct EntityScriptClass
{
    typedef void (*OnCreateType)(MonoObject*, MonoException**);
    typedef void (*OnDestroyType)(MonoObject*, MonoException**);
    typedef void (*OnUpdateType)(MonoObject*, float, MonoException**);

    std::string FullName;
    std::string ClassName;
    std::string NamespaceName;

    MonoClass* Class;
    OnCreateType OnCreateMethod = nullptr;
    OnDestroyType OnDestroyMethod = nullptr;
    OnUpdateType OnUpdateMethod = nullptr;

    void InitClassMethods(MonoImage* image)
    {
        OnCreateMethod = (OnCreateType)GetMethodThunk(image, FullName + ":OnCreate()");
        OnDestroyMethod = (OnDestroyType)GetMethodThunk(image, FullName + ":OnDestroy()");
        OnUpdateMethod = (OnUpdateType)GetMethodThunk(image, FullName + ":OnUpdate()");
    }
};

struct EntityInstance
{
    EntityScriptClass* ScriptClass;

    uint32_t Handle;
    Scene* SceneInstance;

    MonoObject* GetInstance()
    {
        return mono_gchandle_get_target(Handle);
    }
};

MonoDomain* g_MonoDomain = nullptr;

static std::string s_AssemblyPath;
static MonoAssembly* s_AppAssembly = nullptr;
static MonoAssembly* s_CoreAssembly = nullptr;
static MonoImage* s_AppAssemblyImage = nullptr;
MonoImage* g_CoreAssemblyImage = nullptr;

static ScriptModuleFieldMap s_PublicFields;
static std::unordered_map<std::string, EntityScriptClass> s_EntityClassMap;
static std::unordered_map<uint32_t, EntityInstance> s_EntityInstanceMap;

static MonoAssembly* LoadAssemblyFromFile(const std::string& filepath)
{
    if (filepath.empty())
        return nullptr;

    void* data = nullptr;
    size_t dataSize = FileSystem::ReadFile(filepath.c_str(), &data);
    if (dataSize == 0)
        return nullptr;

    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data(reinterpret_cast<char*>(data), (uint32_t)dataSize, true, &status);
    if (status != MONO_IMAGE_OK)
        return nullptr;

    MonoAssembly* assembly = mono_assembly_load_from(image, filepath.c_str(), &status);

    free(data);
    mono_image_close(image);
    return assembly;
}

static MonoAssembly* LoadAssembly(const std::string& filepath)
{
    MonoAssembly* assembly = LoadAssemblyFromFile(filepath);

    if (assembly)
        AB_CORE_INFO("Successfully loaded assembly: {}", filepath);
    else
        AB_CORE_ERROR("Could not load assembly: {}", filepath);

    return assembly;
}

static MonoImage* GetAssemblyImage(MonoAssembly* assembly)
{
    MonoImage* image = mono_assembly_get_image(assembly);
    if (!image)
        AB_CORE_ERROR("Could not load image from assembly.");

    return image;
}

void ScriptEngine::Init()
{
    mono_set_assemblies_path("mono/lib");
    auto domain = mono_jit_init("Amber");

    g_MonoDomain = mono_domain_create_appdomain("AmberRuntime", nullptr);

    s_CoreAssembly = LoadAssembly("assets/scripts/ScriptCore.dll");
    g_CoreAssemblyImage = GetAssemblyImage(s_CoreAssembly);
}

void ScriptEngine::LoadRuntimeAssembly(const std::string& assemblyPath)
{
    if (s_AppAssembly)
    {
        mono_domain_unload(g_MonoDomain);
        mono_assembly_close(s_AppAssembly);

        mono_domain_create_appdomain("AmberRuntime", nullptr);
    }

    s_AssemblyPath = assemblyPath;

    s_AppAssembly = LoadAssembly(s_AssemblyPath);
    s_AppAssemblyImage = GetAssemblyImage(s_AppAssembly);

    ScriptEngineRegistry::RegisterAll();
}

void ScriptEngine::Shutdown()
{
    mono_jit_cleanup(g_MonoDomain);
}

static void* GetMethodThunk(MonoImage* image, const std::string& methodDesc)
{
    MonoMethodDesc* desc = mono_method_desc_new(methodDesc.c_str(), false);
    if (!desc)
    {
        AB_CORE_ERROR("Could not create method description: {}", methodDesc);
        return nullptr;
    }

    MonoMethod* method = mono_method_desc_search_in_image(desc, image);
    if (!method)
    {
        AB_CORE_ERROR("Could not find method: {}", methodDesc);
        return nullptr;
    }

    return mono_method_get_unmanaged_thunk(method);
}

static MonoObject* CallMethod(MonoObject* object, MonoMethod* method, void** params = nullptr)
{
    MonoObject* exception = nullptr;
    MonoObject* result = mono_runtime_invoke(method, object, params, &exception);
    return result;
}

void ScriptEngine::OnCreateEntity(const Entity& entity)
{
    mono_gc_collect(mono_gc_max_generation());

    auto& instance = s_EntityInstanceMap[entity];
    if (instance.ScriptClass->OnCreateMethod)
    {
        MonoException* exception;
        instance.ScriptClass->OnCreateMethod(instance.GetInstance(), &exception);
    }
}

void ScriptEngine::OnDestroyEntity(const Entity& entity)
{
    auto& instance = s_EntityInstanceMap[entity];
    if (instance.ScriptClass->OnDestroyMethod)
    {
        MonoException* exception;
        instance.ScriptClass->OnDestroyMethod(instance.GetInstance(), &exception);
    }

    mono_gc_collect(mono_gc_max_generation());
    mono_gchandle_free(instance.Handle);
    s_EntityInstanceMap.erase(entity);
}

void ScriptEngine::OnUpdateEntity(uint32_t entityID, Timestep ts)
{
    AB_CORE_ASSERT(s_EntityInstanceMap.find(entityID) != s_EntityInstanceMap.end(), "Entity instance not found!");

    mono_gc_collect(mono_gc_max_generation());

    auto& instance = s_EntityInstanceMap[entityID];
    if (instance.ScriptClass->OnUpdateMethod)
    {
        MonoException* exception;
        instance.ScriptClass->OnUpdateMethod(instance.GetInstance(), ts, &exception);
    }
}

static MonoClass* GetClass(MonoImage* image, const EntityScriptClass& scriptClass)
{
    MonoClass* monoClass = mono_class_from_name(image, scriptClass.NamespaceName.c_str(), scriptClass.ClassName.c_str());
    if (!monoClass)
        AB_CORE_ERROR("Class not found: {}", scriptClass.FullName);

    return monoClass;
}

static uint32_t Instantiate(const EntityScriptClass& scriptClass)
{
    MonoObject* object = mono_object_new(g_MonoDomain, scriptClass.Class);
    if (!object)
        AB_CORE_ERROR("Could not instantiate class: {}", scriptClass.FullName);

    mono_runtime_object_init(object);
    uint32_t handle = mono_gchandle_new(object, false);
    return handle;
}

static FieldType MonoTypeToFieldType(MonoType* monoType)
{
    int type = mono_type_get_type(monoType);
    switch (type)
    {
        case MONO_TYPE_BOOLEAN: return FieldType::Bool;
        case MONO_TYPE_I4: return FieldType::Int;
        case MONO_TYPE_U4: return FieldType::UInt;
        case MONO_TYPE_R4: return FieldType::Float;
        case MONO_TYPE_STRING: return FieldType::String;
        case MONO_TYPE_VALUETYPE:
        {
            const char* name = mono_type_get_name(monoType);
            if (strcmp(name, "Amber.Vector2") == 0) return FieldType::Vec2;
            if (strcmp(name, "Amber.Vector3") == 0) return FieldType::Vec3;
            if (strcmp(name, "Amber.Vector4") == 0) return FieldType::Vec4;
        }
    }

    return FieldType::None;
}

void ScriptEngine::OnInitEntity(ScriptComponent& script, uint32_t entityID, uint32_t sceneID)
{
    auto& scriptClass = s_EntityClassMap[script.ModuleName];
    scriptClass.FullName = script.ModuleName;
    if (scriptClass.FullName.find('.') != std::string::npos)
    {
        size_t pos = scriptClass.FullName.rfind('.');
        scriptClass.NamespaceName = scriptClass.FullName.substr(0, pos);
        scriptClass.ClassName = scriptClass.FullName.substr(pos + 1);
    }
    else
    {
        scriptClass.ClassName = scriptClass.FullName;
    }

    scriptClass.Class = GetClass(s_AppAssemblyImage, scriptClass);
    scriptClass.InitClassMethods(s_AppAssemblyImage);

    EntityInstance& entity = s_EntityInstanceMap[entityID];
    entity.ScriptClass = &scriptClass;
    entity.Handle = Instantiate(scriptClass);

    MonoProperty* entityIDProperty = mono_class_get_property_from_name(scriptClass.Class, "EntityID");
    MonoMethod* entityIDSetter = mono_property_get_set_method(entityIDProperty);
    void* params[] = { &entityID };
    CallMethod(entity.GetInstance(), entityIDSetter, params);

    MonoProperty* sceneIDProperty = mono_class_get_property_from_name(scriptClass.Class, "SceneID");
    MonoMethod* sceneIDSetter = mono_property_get_set_method(sceneIDProperty);
    params[0] = { &sceneID };
    CallMethod(entity.GetInstance(), sceneIDSetter, params);

    if (scriptClass.OnCreateMethod)
    {
        MonoException* exception;
        scriptClass.OnCreateMethod(entity.GetInstance(), &exception);
    }

    MonoClassField* field = nullptr;
    void* iter = 0;
    while ((field = mono_class_get_fields(scriptClass.Class, &iter)) != nullptr)
    {
        const char* name = mono_field_get_name(field);
        
        uint32_t flags = mono_field_get_flags(field);
        if (!(flags & MONO_FIELD_ATTR_PUBLIC))
            continue;

        MonoType* monoType = mono_field_get_type(field);
        FieldType type = MonoTypeToFieldType(monoType);

        auto& publicField = s_PublicFields[scriptClass.FullName].emplace_back(name, type);
        publicField.Entity = &entity;
        publicField.ClassField = field;
    }
}

const ScriptModuleFieldMap& ScriptEngine::GetFieldMap()
{
    return s_PublicFields;
}

void PublicField::GetValue_Internal(void* outValue) const
{
    mono_field_get_value(Entity->GetInstance(), ClassField, outValue);
}

void PublicField::SetValue_Internal(void* value) const
{
    mono_field_set_value(Entity->GetInstance(), ClassField, value);
}

}