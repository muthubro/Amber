#include "SceneHierarchyPanel.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include <ImGui/imgui.h>

#include "Editor/EditorLayer.h"

#include "Editor/ImGui/ImGuiUtils.h"

namespace Amber
{
namespace Editor
{

SceneHierarchyPanel::SceneHierarchyPanel(Ref<Scene> context)
{
    SetContext(context);
}

void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
{
    m_Context = context;
    m_EntityMap.clear();
    for (auto& entityID : m_Context->GetAllEntities())
    {
        Entity entity(entityID, m_Context.Raw());
        m_EntityMap[(uint32_t)entity] = EntityData();
        
        auto [translation, orientation, scale] = Math::DecomposeTransform(entity.GetComponent<TransformComponent>());
        m_EntityMap[entity].Orientation = glm::degrees(glm::eulerAngles(orientation));
    }
}

void SceneHierarchyPanel::SetOrientation(const Entity& entity, const glm::quat& orientation)
{
    auto& prevOrientation = m_EntityMap[entity].Orientation;
    prevOrientation += glm::degrees(glm::eulerAngles(orientation));

    if (glm::abs(prevOrientation.x) > 360.0f)
        prevOrientation.x -= std::copysign(360.0f, prevOrientation.x);
    if (glm::abs(prevOrientation.y) > 360.0f)
        prevOrientation.y -= std::copysign(360.0f, prevOrientation.y);
    if (glm::abs(prevOrientation.z) > 360.0f)
        prevOrientation.z -= std::copysign(360.0f, prevOrientation.z);
}

void SceneHierarchyPanel::SetSelection(std::vector<SelectedSubmesh>& selection)
{
    m_SelectionContext = &selection;

    for (const auto& entity : m_Context->GetAllEntities())
        m_EntityMap[(uint32_t)entity].Selected = false;

    for (const auto& selection : *m_SelectionContext)
        m_EntityMap[selection.Entity].Selected = true;
}

void SceneHierarchyPanel::OnImGuiRender()
{
    ImGui::Begin("Scene Hierarchy");

    bool entityClicked = false;
    auto& entities = m_Context->GetAllEntities();
    int32_t i = 0;
    for (auto& entity : entities)
    {
        Entity entity(entity, m_Context.Raw());
        if (m_EntityMap.find(entity) == m_EntityMap.end())
        {
            auto [translation, orientation, scale] = Math::DecomposeTransform(entity.GetComponent<TransformComponent>());
            m_EntityMap[entity] = EntityData{ false, glm::degrees(glm::eulerAngles(orientation)) };
        }
        entityClicked |= DrawEntityNode(entity, i++);
    }

    if (!entityClicked && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
    {
        m_SelectionContext->clear();
        for (auto& pair : m_EntityMap)
            pair.second.Selected = false;
    }

    if (ImGui::BeginPopupContextWindow(nullptr, ImGuiMouseButton_Right, false))
    {
        if (ImGui::MenuItem("Create Entity"))
            m_Context->CreateEntity("Unnamed Entity");

        ImGui::EndPopup();
    }

    ImGui::End();

    ImGui::Begin("Properties");

    if (!m_SelectionContext->empty())
    {
        auto& selection = *(m_SelectionContext->rbegin());
        DrawComponents(selection.Entity);
    }
 
    ImGui::End();
}

bool SceneHierarchyPanel::DrawEntityNode(Entity& entity, int32_t index)
{
    const char* name = entity.GetComponent<TagComponent>().Tag.c_str();

    static int32_t selectionStart = -1;
    static int32_t selectionEnd = -1;

    ImGuiTreeNodeFlags nodeFlags = (m_EntityMap[entity].Selected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool opened = ImGui::TreeNodeEx(name, nodeFlags);
    bool clicked = ImGui::IsItemClicked();
    if (clicked)
    {
        SelectedSubmesh selection{ entity, nullptr, 0.0f };
        auto mesh = entity.GetComponentIfExists<MeshComponent>();
        if (mesh && *mesh)
            selection.Mesh = &mesh->Mesh->GetSubmeshes()[0];

        if (m_SelectionContext->empty())
            selectionStart = -1;

        if (Input::IsKeyPressed(AB_KEY_LEFT_SHIFT) || Input::IsKeyPressed(AB_KEY_RIGHT_SHIFT))
        {
            if (selectionStart == -1)
            {
                m_SelectionContext->clear();
                m_SelectionContext->push_back(selection);
                m_EntityMap[entity].Selected = true;
                selectionStart = selectionEnd = index;
            }
            else if (index != selectionEnd)
            {
                selectionEnd = index;
                auto temp = selectionStart;
                selectionStart = std::min(selectionStart, selectionEnd);
                selectionEnd = std::max(temp, selectionEnd);

                m_SelectionContext->clear();
                for (const auto& entity : m_Context->GetAllEntities())
                    m_EntityMap[(uint32_t)entity].Selected = false;

                const auto& entities = m_Context->GetAllEntities();
                for (int32_t i = selectionStart; i <= selectionEnd; i++)
                {
                    Entity entity_i(entities[i], m_Context.Raw());
                    SelectedSubmesh entitySelection{ entity_i, nullptr, 0.0f };
                    auto meshComponent = entity.GetComponentIfExists<MeshComponent>();
                    if (mesh && *mesh)
                        entitySelection.Mesh = &meshComponent->Mesh->GetSubmeshes()[0];
                    
                    m_SelectionContext->push_back(entitySelection);
                    m_EntityMap[entity_i].Selected = true;
                }
            }
        }
        else
        {
            selectionStart = index;
            selectionEnd = -1;

            if (Input::IsKeyPressed(AB_KEY_LEFT_CONTROL) || Input::IsKeyPressed(AB_KEY_RIGHT_CONTROL))
            {
                auto pos = std::find_if(m_SelectionContext->begin(), m_SelectionContext->end(), SelectedEntityComparator(selection));
                if (pos != m_SelectionContext->end())
                {
                    m_SelectionContext->erase(pos);
                    m_EntityMap[entity].Selected = false;
                }
                else
                {
                    m_SelectionContext->push_back(selection);
                    m_EntityMap[entity].Selected = true;
                }
            }
            else
            {
                m_SelectionContext->clear();
                m_SelectionContext->push_back(selection);

                for (const auto& entity : m_Context->GetAllEntities())
                    m_EntityMap[(uint32_t)entity].Selected = false;
                m_EntityMap[selection.Entity].Selected = true;
            }
        }
    }

    bool entityDeleted = false;
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete"))
            entityDeleted = true;

        ImGui::EndPopup();
    }

    if (opened)
        ImGui::TreePop();

    if (entityDeleted)
    {
        m_EntityMap.erase(entity);
        m_SelectionContext->clear();
        m_Context->DestroyEntity(entity);
    }

    return clicked;
}

void SceneHierarchyPanel::DrawComponents(Entity& entity)
{
    auto tag = entity.GetComponent<TagComponent>();
    Property("", tag.Tag);

    ImGui::SameLine();

    ImGui::TextDisabled("%llx", entity.GetComponent<IDComponent>().ID);

    ImGui::Separator();

    auto& transform = entity.GetComponent<TransformComponent>();
    if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(TransformComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
    {
        BeginPropertyGrid();

        auto [translation, orientationQuat, scale] = Math::DecomposeTransform(transform);
        glm::vec3 orientation = m_EntityMap[entity].Orientation;

        bool changed = false;
        changed |= Property("Translation", translation, -10000.0f, 10000.0f);
        changed |= Property("Rotation", orientation, -360.0f, 360.0f);
        changed |= Property("Scale", scale, 0.001f, 10000.0f);

        if (changed)
        {
            glm::quat rotation(glm::radians(orientation));
            transform.Transform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
            m_EntityMap[entity].Orientation = orientation;
        }

        EndPropertyGrid();
        ImGui::TreePop();
    }
    ImGui::Separator();

    auto mesh = entity.GetComponentIfExists<MeshComponent>();
    if (mesh)
    {
        if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(MeshComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Mesh"))
        {
            BeginPropertyGrid(3);
            
            if (mesh->Mesh)
                Property("File Path", (const char*)mesh->Mesh->GetFilePath().c_str());
            else
                Property("File Path", (const char*)"NULL");

            if (ImGui::Button("...##MeshOpen"))
            {
                std::string filepath = FileSystem::OpenFileDialog("FBX:(*.fbx)", "Select Mesh");
                if (!filepath.empty())
                    mesh->Mesh = Ref<Mesh>::Create(filepath);
            }
            ImGui::NextColumn();

            EndPropertyGrid();
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    auto cameraComponent = entity.GetComponentIfExists<CameraComponent>();
    if (cameraComponent)
    {
        if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(CameraComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
        {
            auto& camera = cameraComponent->Camera;

            int type = (int)camera.GetProjectionType();
            ImGui::RadioButton("Perspective##Projection", &type, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Orthographic##Projection", &type, 1);

            BeginPropertyGrid();
            switch (type)
            {
                case 0:
                {
                    auto data = camera.GetPerspectiveData();
                    data.FOV = glm::degrees(data.FOV);
                    Property("Vertical FOV", data.FOV, 0.0f, 90.0f);
                    Property("Width", data.Width, 0.1f, 10000.0f);
                    Property("Height", data.Height, 0.1f, 10000.0f);
                    Property("Near Clip", data.Near, 0.1f, 100.0f);
                    Property("Far Clip", data.Far, data.Near, 100000.0f);
                    data.FOV = glm::radians(data.FOV);
                    camera.SetPerspective(data);
                    break;
                }

                case 1:
                {
                    auto data = camera.GetOrthographicData();
                    Property("Size", data.Size, 0.1f, 10000.0f);
                    Property("Aspect Ratio", data.AspectRatio, 0.1f, 10.0f);
                    Property("Near Clip", data.Near, -1.0f, 1.0f);
                    Property("Far Clip", data.Far, data.Near, 1.0f);
                    camera.SetOrthographic(data);
                    break;
                }
            }
            EndPropertyGrid();
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    auto script = entity.GetComponentIfExists<ScriptComponent>();
    if (script)
    {
        if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(ScriptComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Script"))
        {
            BeginPropertyGrid();
            std::string oldName = script->ModuleName;
            if (Property("Module Name", script->ModuleName, !ScriptEngine::ModuleExists(script->ModuleName)))
            {
                if (ScriptEngine::ModuleExists(oldName))
                    ScriptEngine::ShutdownScriptEntity(entity, oldName);

                if (ScriptEngine::ModuleExists(script->ModuleName))
                    ScriptEngine::InitScriptEntity(entity);
            }

            if (ScriptEngine::ModuleExists(script->ModuleName))
            {
                auto& instanceData = ScriptEngine::GetEntityInstanceData(m_Context->GetUUID(), entity.GetUUID());
                auto& moduleFieldMap = instanceData.ModuleFieldMap;
                if (moduleFieldMap.find(script->ModuleName) != moduleFieldMap.end())
                {
                    auto& fields = moduleFieldMap.at(script->ModuleName);
                    for (auto& [fieldName, field] : fields)
                    {
                        bool isRuntime = m_Context->IsPlaying() && field.IsRuntimeAvailable();
                        switch (field.Type)
                        {
                            case FieldType::Bool:
                            {
                                bool value = isRuntime ? field.GetRuntimeValue<bool>() : field.GetStoredValue<bool>();
                                if (Property(field.Name.c_str(), value))
                                {
                                    if (isRuntime)
                                        field.SetRuntimeValue(value);
                                    else
                                        field.SetStoredValue(value);
                                }
                                break;
                            }

                            case FieldType::Int:
                            {
                                int32_t value = isRuntime ? field.GetRuntimeValue<int32_t>() : field.GetStoredValue<int32_t>();
                                if (Property(field.Name.c_str(), value))
                                {
                                    if (isRuntime)
                                        field.SetRuntimeValue(value);
                                    else
                                        field.SetStoredValue(value);
                                }
                                break;
                            }

                            case FieldType::UInt:
                            {
                                int32_t value = isRuntime ? field.GetRuntimeValue<int32_t>() : field.GetStoredValue<int32_t>();
                                if (Property(field.Name.c_str(), value, 1, 2048))
                                {
                                    if (isRuntime)
                                        field.SetRuntimeValue(value);
                                    else
                                        field.SetStoredValue(value);
                                }
                                break;
                            }

                            case FieldType::Float:
                            {
                                float value = isRuntime ? field.GetRuntimeValue<float>() : field.GetStoredValue<float>();
                                if (Property(field.Name.c_str(), value, 0.001f, 10.0f, 0.001f))
                                {
                                    if (isRuntime)
                                        field.SetRuntimeValue(value);
                                    else
                                        field.SetStoredValue(value);
                                }
                                break;
                            }

                            case FieldType::Vec2:
                            {
                                glm::vec2 value = isRuntime ? field.GetRuntimeValue<glm::vec2>() : field.GetStoredValue<glm::vec2>();
                                if (Property(field.Name.c_str(), value))
                                {
                                    if (isRuntime)
                                        field.SetRuntimeValue(value);
                                    else
                                        field.SetStoredValue(value);
                                }
                                break;
                            }

                            case FieldType::Vec3:
                            {
                                glm::vec3 value = isRuntime ? field.GetRuntimeValue<glm::vec3>() : field.GetStoredValue<glm::vec3>();
                                if (Property(field.Name.c_str(), value))
                                {
                                    if (isRuntime)
                                        field.SetRuntimeValue(value);
                                    else
                                        field.SetStoredValue(value);
                                }
                                break;
                            }

                            case FieldType::Vec4:
                            {
                                glm::vec4 value = isRuntime ? field.GetRuntimeValue<glm::vec4>() : field.GetStoredValue<glm::vec4>();
                                if (Property(field.Name.c_str(), value))
                                {
                                    if (isRuntime)
                                        field.SetRuntimeValue(value);
                                    else
                                        field.SetStoredValue(value);
                                }
                                break;
                            }
                        }
                    }
                }
            }

            if (ImGui::Button("Run Script"))
                ScriptEngine::OnCreateEntity(entity);
            
            EndPropertyGrid();
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    if (ImGui::Button("+ Add Component"))
        ImGui::OpenPopup("AddComponentPanel");

    if (ImGui::BeginPopup("AddComponentPanel"))
    {
        if (!entity.HasComponent<MeshComponent>())
        {
            if (ImGui::Button("Mesh"))
            {
                entity.AddComponent<MeshComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!entity.HasComponent<CameraComponent>())
        {
            if (ImGui::Button("Camera"))
            {
                entity.AddComponent<CameraComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!entity.HasComponent<ScriptComponent>())
        {
            if (ImGui::Button("Script"))
            {
                entity.AddComponent<ScriptComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

} // Editor
} // Amber