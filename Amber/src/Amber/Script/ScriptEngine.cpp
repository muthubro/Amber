#include "abpch.h"
#include "ScriptEngine.h"

#include <ImGui/imgui.h>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/mono-gc.h>

#include "Amber/Core/FileSystem.h"

#include "ScriptEngineRegistry.h"

namespace Amber
{

MonoDomain* g_MonoDomain = nullptr;

static std::string s_AssemblyPath;
static MonoAssembly* s_AppAssembly = nullptr;
static MonoAssembly* s_CoreAssembly = nullptr;
static MonoImage* s_AppAssemblyImage = nullptr;
MonoImage* g_CoreAssemblyImage = nullptr;

static EntityInstanceMap s_EntityInstanceMap;

static Ref<Scene> s_SceneContext = nullptr;

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
        OnUpdateMethod = (OnUpdateType)GetMethodThunk(image, FullName + ":OnUpdate(single)");
    }
};

MonoObject* EntityInstance::GetInstance()
{
    return mono_gchandle_get_target(Handle);
}

static std::unordered_map<std::string, EntityScriptClass> s_EntityClassMap;

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

void ScriptEngine::LoadRuntimeAssembly(const std::string& assemblyPath, const Scene& scene)
{
    if (s_AppAssembly)
    {
        mono_domain_unload(g_MonoDomain);
        mono_assembly_close(s_AppAssembly);

        mono_domain_create_appdomain("AmberRuntime", nullptr);

        s_CoreAssembly = LoadAssembly("assets/scripts/ScriptCore.dll");
        g_CoreAssemblyImage = GetAssemblyImage(s_CoreAssembly);
    }

    s_AssemblyPath = assemblyPath;

    s_AppAssembly = LoadAssembly(s_AssemblyPath);
    s_AppAssemblyImage = GetAssemblyImage(s_AppAssembly);

    ScriptEngineRegistry::RegisterAll();

    if (!s_EntityInstanceMap.empty())
    {
        if (s_EntityInstanceMap.find(scene.GetUUID()) != s_EntityInstanceMap.end())
        {
            auto& instanceMap = s_EntityInstanceMap[scene.GetUUID()];
            for (auto& [entityID, instanceData] : instanceMap)
            {
                const EntityMap& entityMap = scene.GetEntityMap();
                AB_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid entity ID or entity doesn't exist in scene!");
                InitScriptEntity(entityMap.at(entityID));
            }
        }
    }
}

void ScriptEngine::OnSceneDestruct(UUID sceneID)
{
    if (s_EntityInstanceMap.find(sceneID) != s_EntityInstanceMap.end())
    {
        s_EntityInstanceMap[sceneID].clear();
        s_EntityInstanceMap.erase(sceneID);
    }
}

void ScriptEngine::OnScriptComponentDestroyed(UUID sceneID, UUID entityID)
{
    AB_CORE_ASSERT(s_EntityInstanceMap.find(sceneID) != s_EntityInstanceMap.end(), "Invalid Scene ID!");
    auto& entityMap = s_EntityInstanceMap[sceneID];

    AB_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid Entity ID!");
    entityMap.erase(entityID);
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
        return nullptr;

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
    OnCreateEntity(entity.GetSceneUUID(), entity.GetUUID());
}

void ScriptEngine::OnCreateEntity(UUID sceneID, UUID entityID)
{
    EntityInstance& instance = GetEntityInstanceData(sceneID, entityID).Instance;
    
    if (instance.ScriptClass->OnCreateMethod)
    {
        MonoException* exception;
        instance.ScriptClass->OnCreateMethod(instance.GetInstance(), &exception);
    }
}

void ScriptEngine::OnDestroyEntity(UUID sceneID, UUID entityID)
{
    EntityInstance& instance = GetEntityInstanceData(sceneID, entityID).Instance;
    if (instance.ScriptClass->OnDestroyMethod)
    {
        MonoException* exception;
        instance.ScriptClass->OnDestroyMethod(instance.GetInstance(), &exception);
    }

    mono_gc_collect(mono_gc_max_generation());
    while (mono_gc_pending_finalizers());
    mono_gchandle_free(instance.Handle);
    s_EntityInstanceMap[sceneID].erase(entityID);
}

void ScriptEngine::OnUpdateEntity(UUID sceneID, UUID entityID, Timestep ts)
{
    EntityInstance& instance = GetEntityInstanceData(sceneID, entityID).Instance;

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
        case MONO_TYPE_I4:      return FieldType::Int;
        case MONO_TYPE_U4:      return FieldType::UInt;
        case MONO_TYPE_R4:      return FieldType::Float;
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

bool ScriptEngine::ModuleExists(const std::string& moduleName)
{
    std::string ClassName, NamespaceName;
    if (moduleName.find('.') != std::string::npos)
    {
        size_t pos = moduleName.rfind('.');
        NamespaceName = moduleName.substr(0, pos);
        ClassName = moduleName.substr(pos + 1);
    }
    else
    {
        ClassName = moduleName;
    }

    MonoClass* monoClass = mono_class_from_name(s_AppAssemblyImage, NamespaceName.c_str(), ClassName.c_str());
    return monoClass;
}

void ScriptEngine::InitScriptEntity(const Entity& entity)
{
    UUID entityID = entity.GetUUID();
    auto& moduleName = entity.GetComponent<ScriptComponent>().ModuleName;
    if (moduleName.empty())
        return;

    if (!ModuleExists(moduleName))
    {
        AB_CORE_ERROR("Entity references non-existent script module '{0}'", moduleName);
        return;
    }

    auto& scriptClass = s_EntityClassMap[moduleName];
    scriptClass.FullName = moduleName;
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

    EntityInstanceData& instanceData = s_EntityInstanceMap[entity.GetSceneUUID()][entityID];

    EntityInstance& instance = instanceData.Instance;
    instance.ScriptClass = &scriptClass;

    ScriptModuleFieldMap& moduleFieldMap = instanceData.ModuleFieldMap;
    auto& fieldMap = moduleFieldMap[moduleName];

    FieldMap oldFields;
    oldFields.reserve(fieldMap.size());
    for (auto& [fieldName, field] : fieldMap)
        oldFields.emplace(fieldName, std::move(field));
    fieldMap.clear();

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

        MonoCustomAttrInfo* attributes = mono_custom_attrs_from_field(scriptClass.Class, field);

        if (oldFields.find(name) != oldFields.end())
        {
            fieldMap.emplace(name, std::move(oldFields.at(name)));
        }
        else
        {
            PublicField publicField = { name, type };
            publicField.Entity = &instance;
            publicField.ClassField = field;
            fieldMap.emplace(name, std::move(publicField));
        }
    }
}

void ScriptEngine::ShutdownScriptEntity(const Entity& entity, const std::string& moduleName)
{
    EntityInstanceData& instanceData = GetEntityInstanceData(entity.GetSceneUUID(), entity.GetUUID());
    ScriptModuleFieldMap& moduleFieldMap = instanceData.ModuleFieldMap;
    if (moduleFieldMap.find(moduleName) != moduleFieldMap.end())
        moduleFieldMap.erase(moduleName);
}

void ScriptEngine::InstantiateEntityClass(const Entity& entity)
{
    UUID entityID = entity.GetUUID();
    auto& moduleName = entity.GetComponent<ScriptComponent>().ModuleName;

    EntityInstanceData& instanceData = GetEntityInstanceData(entity.GetSceneUUID(), entityID);
    EntityInstance& instance = instanceData.Instance;
    instance.Handle = Instantiate(*instance.ScriptClass);

    MonoProperty* entityIDProperty = mono_class_get_property_from_name(instance.ScriptClass->Class, "ID");
    MonoMethod* entityIDSetter = mono_property_get_set_method(entityIDProperty);
    void* params[] = { &entityID };
    CallMethod(instance.GetInstance(), entityIDSetter, params);

    auto& moduleFieldMap = instanceData.ModuleFieldMap;
    if (moduleFieldMap.find(moduleName) != moduleFieldMap.end())
    {
        auto& fieldMap = moduleFieldMap[moduleName];
        for (auto& [fieldName, field] : fieldMap)
            field.CopyStorageToRuntime();
    }

    OnCreateEntity(entity);
}

void ScriptEngine::CopyEntityScriptData(UUID dstScene, UUID srcScene)
{
    AB_CORE_ASSERT(s_EntityInstanceMap.find(dstScene) != s_EntityInstanceMap.end());
    AB_CORE_ASSERT(s_EntityInstanceMap.find(srcScene) != s_EntityInstanceMap.end());

    auto& dstEntityMap = s_EntityInstanceMap[dstScene];
    auto& srcEntityMap = s_EntityInstanceMap[srcScene];

    for (auto& [entityID, instanceData] : srcEntityMap)
    {
        for (auto& [moduleName, srcFieldMap] : instanceData.ModuleFieldMap)
        {
            auto& dstModuleFieldMap = dstEntityMap[entityID].ModuleFieldMap;
            AB_CORE_ASSERT(dstModuleFieldMap.find(moduleName) != dstModuleFieldMap.end());
            
            for (auto& [fieldName, field] : srcFieldMap)
            {
                auto& dstFieldMap = dstModuleFieldMap[moduleName];
                AB_CORE_ASSERT(dstFieldMap.find(fieldName) != dstFieldMap.end());
                dstFieldMap.at(fieldName).StoredValue = field.StoredValue;
            }
        }
    }
}

Ref<Scene> ScriptEngine::GetSceneContext()
{
    return s_SceneContext;
}

void ScriptEngine::SetSceneContext(const Ref<Scene>& scene)
{
    s_SceneContext = scene;
}

EntityInstanceMap& ScriptEngine::GetEntityInstanceMap()
{
    return s_EntityInstanceMap;
}

EntityInstanceData& ScriptEngine::GetEntityInstanceData(UUID sceneID, UUID entityID)
{
    AB_CORE_ASSERT(s_EntityInstanceMap.find(sceneID) != s_EntityInstanceMap.end(), "Invalid Scene ID!");
    auto& entityMap = s_EntityInstanceMap[sceneID];

    AB_CORE_ASSERT(entityMap.find(entityID) != entityMap.end(), "Invalid Entity ID!");
    return entityMap[entityID];
}

void ScriptEngine::OnImGuiRender()
{
    ImGui::Begin("Script Engine Debug");

    for (auto& [sceneID, entityMap] : s_EntityInstanceMap)
    {
        if (ImGui::TreeNode((void*)(uint64_t)sceneID, "Scene (%llx)", sceneID))
        {
            Ref<Scene> scene = Scene::GetScene(sceneID);
            for (auto& [entityID, instanceData] : entityMap)
            {
                Entity entity = scene->GetEntityMap().at(entityID);
                auto& entityName = entity.GetComponent<TagComponent>().Tag;

                if (ImGui::TreeNode((void*)(uint64_t)entityID, "%s (%llx)", entityName.c_str(), entityID))
                {
                    for (auto& [moduleName, fieldMap] : instanceData.ModuleFieldMap)
                    {
                        if (ImGui::TreeNode(moduleName.c_str()))
                        {
                            for (auto& [fieldName, field] : fieldMap)
                            {
                                if (ImGui::TreeNodeEx((void*)fieldName.c_str(), ImGuiTreeNodeFlags_Leaf, fieldName.c_str()))
                                    ImGui::TreePop();
                            }

                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

PublicField::PublicField(const std::string& name, FieldType type)
    : Name(name), Type(type)
{
    StoredValue.Allocate(PublicField::GetFieldSize(type));
}

PublicField::PublicField(PublicField&& other)
{
    Name = std::move(other.Name);
    Type = other.Type;
    Entity = other.Entity;
    ClassField = other.ClassField;
    StoredValue = std::move(other.StoredValue);

    other.Entity = nullptr;
    other.ClassField = nullptr;
}

void PublicField::CopyStorageToRuntime()
{
    mono_field_set_value(Entity->GetInstance(), ClassField, StoredValue.Data);
}

bool PublicField::IsRuntimeAvailable() const
{
    return Entity && Entity->Handle;
}

uint32_t PublicField::GetFieldSize(FieldType type)
{
    switch (type)
    {
        case FieldType::Bool:   return 1;
        case FieldType::Int:    return 4;
        case FieldType::UInt:   return 4;
        case FieldType::Float:  return 4;
        case FieldType::Vec2:   return 4 * 2;
        case FieldType::Vec3:   return 4 * 3;
        case FieldType::Vec4:   return 4 * 4;
    }

    AB_CORE_ASSERT(false, "Unknown field type!");
    return 0;
}

PublicField& PublicField::operator=(PublicField&& other)
{
    Name = std::move(other.Name);
    Type = other.Type;
    Entity = other.Entity;
    ClassField = other.ClassField;
    StoredValue = std::move(other.StoredValue);

    other.Entity = nullptr;
    other.ClassField = nullptr;

    return *this;
}

void PublicField::GetRuntimeValue_Internal(void* outValue) const
{
    mono_field_get_value(Entity->GetInstance(), ClassField, outValue);
}

void PublicField::SetRuntimeValue_Internal(void* value)
{
    mono_field_set_value(Entity->GetInstance(), ClassField, value);
}

void PublicField::GetStoredValue_Internal(void* outValue) const
{
    StoredValue.Read(outValue, StoredValue.Size);
}

void PublicField::SetStoredValue_Internal(void* value)
{
    StoredValue.Write(value, StoredValue.Size);
}

}