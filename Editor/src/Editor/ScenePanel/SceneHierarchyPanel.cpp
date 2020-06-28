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

static std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4& transform)
{
    glm::vec3 translationVector, skew, scaleVector;
    glm::vec4 perspective;
    glm::quat rotationVector;
    
    glm::decompose(transform, scaleVector, rotationVector, translationVector, skew, perspective);

    return { translationVector, rotationVector, scaleVector };
}

SceneHierarchyPanel::SceneHierarchyPanel(Ref<Scene> context)
    : m_Context(context)
{
    for (auto& entityID : context->GetAllEntities())
    {
        Entity entity(entityID, context.Raw());

        m_EntityMap[(uint32_t)entity] = EntityData();
        auto [translation, orientation, scale] = GetTransformDecomposition(entity.GetComponent<TransformComponent>());
        m_EntityMap[entity].Orientation = glm::degrees(glm::eulerAngles(orientation));
    }
}

void SceneHierarchyPanel::SetOrientation(const Entity& entity, const glm::quat& orientation)
{
    auto& prevOrientation = m_EntityMap[entity].Orientation;
    prevOrientation += glm::eulerAngles(orientation);

    prevOrientation = glm::degrees(prevOrientation);

    if (glm::abs(prevOrientation.x) > 360.0f)
        prevOrientation.x -= std::copysign(360.0f, prevOrientation.x);
    if (glm::abs(prevOrientation.y) > 360.0f)
        prevOrientation.y -= std::copysign(360.0f, prevOrientation.y);
    if (glm::abs(prevOrientation.z) > 360.0f)
        prevOrientation.z -= std::copysign(360.0f, prevOrientation.z);

    prevOrientation = glm::radians(prevOrientation);
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
            auto [translation, orientation, scale] = GetTransformDecomposition(entity.GetComponent<TransformComponent>());
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
    const char* name = "Unnamed Entity";
    auto tag = entity.GetComponentIfExists<TagComponent>();
    if (tag)
        name = tag->Tag.c_str();

    static int32_t selectionStart = -1;
    static int32_t selectionEnd = -1;

    ImGuiTreeNodeFlags nodeFlags = (m_EntityMap[entity].Selected ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool opened = ImGui::TreeNodeEx(name, nodeFlags);
    bool clicked = ImGui::IsItemClicked();
    if (clicked)
    {
        SelectedSubmesh selection{ entity, nullptr, 0.0f };
        auto mesh = entity.GetComponentIfExists<MeshComponent>();
        if (mesh)
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
                    if (mesh)
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
        m_Context->DeleteEntity(entity);
    }

    return clicked;
}

void SceneHierarchyPanel::DrawComponents(Entity& entity)
{
    auto tag = entity.GetComponentIfExists<TagComponent>();
    if (tag)
    {
        BeginPropertyGrid();
        
        Property("", tag->Tag);

        EndPropertyGrid();
        ImGui::Separator();
    }

    auto& transform = entity.GetComponent<TransformComponent>();
    if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(TransformComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
    {
        BeginPropertyGrid();
            
        auto [translation, orientationQuat, scale] = GetTransformDecomposition(transform);
        glm::vec3 orientation = glm::degrees(m_EntityMap[entity].Orientation);

        bool changed = false;
        changed |= Property("Translation", translation, -100.0f, 100.0f);
        changed |= Property("Rotation", orientation, -360.0f, 360.0f);
        changed |= Property("Scale", scale, 0.001f, 100.0f);

        if (changed)
        {
            orientation = glm::radians(orientation);
            glm::quat rotation(orientation);

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
            BeginPropertyGrid();
            
            if (mesh->Mesh)
                Property("File Path", (const char*)mesh->Mesh->GetFilePath().c_str());
            else
                Property("File Path", (const char*)"NULL");

            EndPropertyGrid();
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    auto camera = entity.GetComponentIfExists<CameraComponent>();
    if (camera)
    {
        if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(CameraComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
        {
            BeginPropertyGrid();
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
            Property("", script->ModuleName.c_str());

            auto& fieldMap = ScriptEngine::GetFieldMap();
            if (fieldMap.find(script->ModuleName) != fieldMap.end())
            {
                auto& fields = fieldMap.at(script->ModuleName);
                for (auto& field : fields)
                {
                    switch (field.Type)
                    {
                        case FieldType::Bool:
                        {
                            bool value = field.GetValue<bool>();
                            if (Property(field.Name.c_str(), value))
                                field.SetValue(value);
                            break;
                        }

                        case FieldType::Int:
                        {
                            int32_t value = field.GetValue<int32_t>();
                            if (Property(field.Name.c_str(), value))
                                field.SetValue(value);
                            break;
                        }

                        case FieldType::UInt:
                        {
                            uint32_t value = field.GetValue<uint32_t>();
                            if (Property(field.Name.c_str(), value, 1, 2048))
                                field.SetValue(value);
                            break;
                        }

                        case FieldType::Float:
                        {
                            float value = field.GetValue<float>();
                            if (Property(field.Name.c_str(), value, 0.001f, 10.0f, 0.001f))
                                field.SetValue(value);
                            break;
                        }

                        case FieldType::Vec2:
                        {
                            glm::vec2 value = field.GetValue<glm::vec2>();
                            if (Property(field.Name.c_str(), value))
                                field.SetValue(value);
                            break;
                        }

                        case FieldType::Vec3:
                        {
                            glm::vec3 value = field.GetValue<glm::vec3>();
                            if (Property(field.Name.c_str(), value))
                                field.SetValue(value);
                            break;
                        }

                        case FieldType::Vec4:
                        {
                            glm::vec4 value = field.GetValue<glm::vec4>();
                            if (Property(field.Name.c_str(), value))
                                field.SetValue(value);
                            break;
                        }

                        case FieldType::String:
                        {
                            char* value = field.GetValue<char*>();
                            if (Property(field.Name.c_str(), value))
                                field.SetValue(value);
                            break;
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
}

} // Editor
} // Amber