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

        auto scriptComponent = entity.GetComponentIfExists<ScriptComponent>();
        if (scriptComponent)
        {
            auto& moduleName = scriptComponent->ModuleName;
            auto& fieldMap = ScriptEngine::GetEntityInstanceData(m_Context->GetUUID(), entity.GetUUID()).ModuleFieldMap[moduleName];
            auto& editorFieldMap = m_EntityMap[entity].ScriptFields[moduleName];

            for (auto& [fieldName, field] : fieldMap)
            {
                auto& editorData = editorFieldMap[fieldName];
                auto data = field.GetEditorData(moduleName);

                if (!data.Name.empty())
                    editorData.Name = data.Name;
                else
                    editorData.Name = fieldName;
                editorData.Min = data.Min;
                editorData.Max = data.Max;
                editorData.Step = data.Step;
            }
        }
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
        if (ImGui::BeginMenu("Add Entity"))
        {
            if (ImGui::MenuItem("Empty"))
                m_Context->CreateEntity("Unnamed Entity");

            ImGui::Separator();

            if (ImGui::MenuItem("Cube"))
            {
                auto entity = m_Context->CreateEntity("Cube");
                entity.AddComponent<MeshComponent>(MeshFactory::Cube(glm::vec3(0.0f), 1.0f));
            }

            if (ImGui::MenuItem("Sphere"))
            {
                auto entity = m_Context->CreateEntity("Sphere");
                entity.AddComponent<MeshComponent>(MeshFactory::Sphere(glm::vec3(0.0f), 1.0f));
            }

            if (ImGui::MenuItem("Plane"))
            {
                auto entity = m_Context->CreateEntity("Plane");
                entity.AddComponent<MeshComponent>(MeshFactory::Plane(1.0f, 1.0f));
            }

            ImGui::EndMenu();
        }

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
                    auto meshComponent = entity_i.GetComponentIfExists<MeshComponent>();
                    if (meshComponent)
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

    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete"))
        {
            m_EntityMap.erase(entity);
            m_SelectionContext->clear();
            m_Context->DestroyEntity(entity);
        }

        ImGui::EndPopup();
    }

    if (opened)
        ImGui::TreePop();

    return clicked;
}

template<typename T, typename UIFunction>
static void DrawComponent(const std::string& name, Entity& entity, UIFunction func)
{
    auto component = entity.GetComponentIfExists<T>();
    if (component)
    {
        bool remove = false;
        if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(T).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, name.c_str()))
        {
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Remove"))
                    remove = true;

                ImGui::EndPopup();
            }

            func(*component);

            ImGui::TreePop();
        }
        if (remove)
            entity.RemoveComponent<T>();

        ImGui::Separator();
    }
}

void SceneHierarchyPanel::DrawComponents(Entity& entity)
{
    auto& tagComponent = entity.GetComponent<TagComponent>();

    BeginPropertyGrid(2);
    ImGui::PushItemWidth(-1);
    Property("", tagComponent.Tag);
    ImGui::TextDisabled("%llx", entity.GetComponent<IDComponent>().ID);
    EndPropertyGrid();

    ImGui::Separator();

    auto& transformComponent = entity.GetComponent<TransformComponent>();
    if (ImGui::TreeNodeEx((void*)((uint32_t)entity | typeid(TransformComponent).hash_code()), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
    {
        BeginPropertyGrid();

        auto [translation, orientationQuat, scale] = Math::DecomposeTransform(transformComponent);
        glm::vec3 orientation = m_EntityMap[entity].Orientation;

        bool changed = false;
        changed |= Property("Translation", translation, -10000.0f, 10000.0f);
        changed |= Property("Rotation", orientation, -360.0f, 360.0f);
        changed |= Property("Scale", scale, 0.001f, 10000.0f);

        if (changed)
        {
            glm::quat rotation(glm::radians(orientation));
            transformComponent.Transform = glm::translate(glm::mat4(1.0f), translation) * glm::toMat4(rotation) * glm::scale(glm::mat4(1.0f), scale);
            m_EntityMap[entity].Orientation = orientation;
        }

        EndPropertyGrid();
        ImGui::TreePop();
    }
    ImGui::Separator();

    DrawComponent<MeshComponent>("Mesh", entity, [](MeshComponent& component) {
        BeginPropertyGrid(3);

        if (component.Mesh)
            Property("File Path", (const char*)component.Mesh->GetAssetPath().c_str());
        else
            Property("File Path", (const char*)"NULL");

        if (ImGui::Button("...##MeshOpen"))
        {
            std::string filepath = FileSystem::OpenFileDialog("FBX (*.fbx):*.fbx", "Select Mesh");
            if (!filepath.empty())
                component.Mesh = Ref<Mesh>::Create(filepath);
        }
        ImGui::NextColumn();

        EndPropertyGrid();
    });

    DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component) {
        BeginPropertyGrid(3);

        if (component.Texture)
            Property("File Path", (const char*)component.Texture->GetAssetPath().c_str());
        else
            Property("File Path", (const char*)"NULL");
        
        if (ImGui::Button("...##TextureOpen"))
        {
            std::string filepath = FileSystem::OpenFileDialog("PNG (*.png):*.png", "Select Texture");
            if (!filepath.empty())
                component.Texture = Texture2D::Create(filepath);
        }
        ImGui::NextColumn();
        
        EndPropertyGrid();
        
        // TODO: Fix this
        BeginPropertyGrid(2, 19287319);
        
        for (uint32_t i = 0; i < 4; i++)
            Property("Texture Coord " + std::to_string(i + 1), component.TexCoords[i], 0.0f, 1.0f);
        
        Property("Tiling Factor", component.TilingFactor, 0.0f, 100.0f);

        // TODO: Fix this too
        static bool spriteGradient = false;
        if (spriteGradient)
        {
            for (uint32_t i = 0; i < 4; i++)
                Property("Color " + std::to_string(i), component.Color[i], PropertyFlags::ColorProperty);
        }
        else
        {
            glm::vec4 color = component.Color[0];
            Property("Color", color, PropertyFlags::ColorProperty);
            for (uint32_t i = 0; i < 4; i++)
                component.Color[i] = color;
        }
        
        Property("Color Gradient", spriteGradient);

        EndPropertyGrid();
    });

    DrawComponent<CameraComponent>("Camera", entity, [&context = m_Context](CameraComponent& component) {
        auto& camera = component.Camera;

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
                Property("Near Clip", data.Near, 0.1f, 100000.0f);
                Property("Far Clip", data.Far, data.Near, 100000.0f);
                data.FOV = glm::radians(data.FOV);
                camera.SetPerspective(data);
                break;
            }

            case 1:
            {
                auto data = camera.GetOrthographicData();
                Property("Size", data.Size, 0.1f, 10000.0f);
                Property("Near Clip", data.Near, 0.1f, 100000.0f);
                Property("Far Clip", data.Far, data.Near, 100000.0f);
                camera.SetOrthographic(data);
                break;
            }
        }
        EndPropertyGrid();

        if (!component.Primary)
        {
            if (ImGui::Button("Set as primary"))
            {
                component.Primary = true;
                auto camera = context->GetMainCameraEntity();
                camera.GetComponent<CameraComponent>().Primary = false;
            }
        }
    });

    DrawComponent<ScriptComponent>("Script", entity, [entity, context = m_Context, &entityMap = m_EntityMap](ScriptComponent& component) {
        BeginPropertyGrid();
        std::string oldName = component.ModuleName;
        if (Property("Module Name", component.ModuleName, !ScriptEngine::ModuleExists(component.ModuleName)))
        {
            if (ScriptEngine::ModuleExists(oldName))
                ScriptEngine::ShutdownScriptEntity(entity, oldName);

            if (ScriptEngine::ModuleExists(component.ModuleName))
            {
                ScriptEngine::InitScriptEntity(entity);

                auto& moduleName = component.ModuleName;
                auto& fieldMap = ScriptEngine::GetEntityInstanceData(context->GetUUID(), entity.GetUUID()).ModuleFieldMap[moduleName];
                auto& editorFieldMap = entityMap[entity].ScriptFields[moduleName];

                for (auto& [fieldName, field] : fieldMap)
                {
                    auto& editorData = editorFieldMap[fieldName];
                    auto data = field.GetEditorData(moduleName);

                    if (!data.Name.empty())
                        editorData.Name = data.Name;
                    else
                        editorData.Name = fieldName;
                    editorData.Min = data.Min;
                    editorData.Max = data.Max;
                    editorData.Step = data.Step;
                }
            }
        }

        if (ScriptEngine::ModuleExists(component.ModuleName))
        {
            auto& instanceData = ScriptEngine::GetEntityInstanceData(context->GetUUID(), entity.GetUUID());
            auto& moduleFieldMap = instanceData.ModuleFieldMap;
            if (moduleFieldMap.find(component.ModuleName) != moduleFieldMap.end())
            {
                auto& fields = moduleFieldMap.at(component.ModuleName);
                auto& editorFieldMap = entityMap[entity].ScriptFields[component.ModuleName];
                for (auto& [fieldName, field] : fields)
                {
                    bool isRuntime = context->IsPlaying() && field.IsRuntimeAvailable();
                    auto& fieldData = editorFieldMap[fieldName];
                    switch (field.Type)
                    {
                        case FieldType::Bool:
                        {
                            bool value = isRuntime ? field.GetRuntimeValue<bool>() : field.GetStoredValue<bool>();
                            if (Property(fieldData.Name.c_str(), value))
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
                            if (Property(fieldData.Name.c_str(), value, (int32_t)fieldData.Min, (int32_t)fieldData.Max, (int32_t)fieldData.Step))
                            {
                                value = glm::clamp(value, (int32_t)fieldData.Min, (int32_t)fieldData.Max);
                                if (isRuntime)
                                    field.SetRuntimeValue(value);
                                else
                                    field.SetStoredValue(value);
                            }
                            break;
                        }

                        case FieldType::UInt:
                        {
                            uint32_t value = isRuntime ? field.GetRuntimeValue<uint32_t>() : field.GetStoredValue<uint32_t>();
                            if (Property(fieldData.Name.c_str(), value, (uint32_t)fieldData.Min, (uint32_t)fieldData.Max, (uint32_t)fieldData.Step))
                            {
                                value = glm::clamp(value, (uint32_t)fieldData.Min, (uint32_t)fieldData.Max);
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
                            if (Property(fieldData.Name.c_str(), value, (float)fieldData.Min, (float)fieldData.Max, (float)fieldData.Step))
                            {
                                value = glm::clamp(value, (float)fieldData.Min, (float)fieldData.Max);
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
                            if (Property(fieldData.Name.c_str(), value, (float)fieldData.Min, (float)fieldData.Max, (float)fieldData.Step))
                            {
                                value = glm::clamp(value, (float)fieldData.Min, (float)fieldData.Max);
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
                            if (Property(fieldData.Name.c_str(), value, (float)fieldData.Min, (float)fieldData.Max, (float)fieldData.Step))
                            {
                                value = glm::clamp(value, (float)fieldData.Min, (float)fieldData.Max);
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
                            if (Property(fieldData.Name.c_str(), value, (float)fieldData.Min, (float)fieldData.Max, (float)fieldData.Step))
                            {
                                value = glm::clamp(value, (float)fieldData.Min, (float)fieldData.Max);
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

        EndPropertyGrid();
    });

    DrawComponent<RigidBody2DComponent>("Rigid Body 2D", entity, [](RigidBody2DComponent& component) {
        std::array<const char*, 3> bodyTypes{ "Static", "Dynamic", "Kinematic" };

        ImGui::Text("Body Type");
        ImGui::SameLine();

        uint32_t currentType = (uint32_t)component.BodyType;
        if (ImGui::BeginCombo("##RigidBody2DType", bodyTypes[currentType]))
        {
            for (uint32_t i = 0; i < bodyTypes.size(); i++)
            {
                bool selected = i == currentType;
                if (ImGui::Selectable(bodyTypes[i], &selected))
                    component.BodyType = (RigidBody2DComponent::Type)i;
            }

            ImGui::EndCombo();
        }

        if (component.BodyType == RigidBody2DComponent::Type::Dynamic)
        {
            BeginPropertyGrid();

            Property("Density", component.Density, 0.0f, 100.0f);
            Property("Friction", component.Friction, 0.0f, 1.0f, 0.01f);
            Property("Restitution", component.Restitution, 0.0f, 1.0f, 0.01f);

            EndPropertyGrid();
        }
    });

    DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](BoxCollider2DComponent& component) {
        BeginPropertyGrid();

        Property("Offset", component.Offset);
        Property("Size", component.Size);

        EndPropertyGrid();
    });

    DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](CircleCollider2DComponent& component) {
        BeginPropertyGrid();

        Property("Offset", component.Offset);
        Property("Radius", component.Radius);

        EndPropertyGrid();
    });

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

        if (!entity.HasComponent<SpriteRendererComponent>())
        {
            if (ImGui::Button("Sprite Renderer"))
            {
                entity.AddComponent<SpriteRendererComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!entity.HasComponent<CameraComponent>())
        {
            if (ImGui::Button("Camera"))
            {
                bool primary = true;
                if (m_Context->GetMainCameraEntity())
                    primary = false;
                entity.AddComponent<CameraComponent>(primary);
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

        if (!entity.HasComponent<RigidBody2DComponent>())
        {
            if (ImGui::Button("Rigid Body 2D"))
            {
                entity.AddComponent<RigidBody2DComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!entity.HasComponent<BoxCollider2DComponent>())
        {
            if (ImGui::Button("Box Collider 2D"))
            {
                entity.AddComponent<BoxCollider2DComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        if (!entity.HasComponent<CircleCollider2DComponent>())
        {
            if (ImGui::Button("Circle Collider 2D"))
            {
                entity.AddComponent<CircleCollider2DComponent>();
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

} // Editor
} // Amber