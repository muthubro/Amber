#include "EditorLayer.h"

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include "Amber/ImGui/ImGuizmo.h"

namespace Amber
{

EditorLayer::EditorLayer()
    : Layer("Editor Layer")
{
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::OnAttach()
{
    // ImGui Colors
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f); // Window background
    colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.5f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.3f, 0.3f, 0.3f, 0.5f); // Widget backgrounds
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.4f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 0.6f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.4f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.7f, 0.7f, 0.7f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.7f, 0.7f, 0.7f, 0.8f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.5f, 0.52f, 1.0f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.5f, 0.5f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.6f, 0.15f, 1.0f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
    colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.6f, 0.6f, 1.0f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);

    auto environment = Environment::Load("assets/env/birchwood_4k.hdr");

    m_Scene = Ref<Scene>::Create("Model Scene");
    m_Scene->SetCamera(Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)));
    m_Scene->SetEnvironment(environment);

    m_MeshEntity = m_Scene->CreateEntity("Test entity");

    auto mesh = Ref<Mesh>::Create("assets/meshes/TestScene.fbx");
    m_MeshEntity->SetMesh(mesh);

    m_MeshMaterial = mesh->GetMaterial();

    auto& light = m_Scene->GetLight();
    light.Direction = { -0.5f, 0.5f, 1.0f };
    light.Radiance = { 1.0f, 1.0f, 1.0f };

    m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnEvent(Event& e)
{
    if (m_AllowViewportCameraEvents)
        m_Scene->OnEvent(e);

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseButtonPressedEvent>(AB_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    dispatcher.Dispatch<KeyPressedEvent>(AB_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
}

void EditorLayer::OnUpdate(Timestep ts)
{
    RenderCommand::SetClearColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    RenderCommand::Clear();

    auto& options = SceneRenderer::GetOptions();
    options.ShowGrid = m_ShowGrid;
    options.GridResolution = m_GridResolution;
    options.GridScale = m_GridScale;

    m_Scene->OnUpdate(ts);

    if (m_EnableOverlay && !m_SelectionContext.empty())
    {
        Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
        Renderer2D::BeginScene(m_Scene->GetCamera().GetViewProjection());

        if (m_SelectionMode == SelectionMode::Entity)
        {
            auto& selection = m_SelectionContext[0];
            Renderer::DrawAABB(selection.Entity->GetBoundingBox(), selection.Entity->GetTransform(), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
        }
        else if (m_SelectionMode == SelectionMode::Submesh)
        {
            for (auto& selection : m_SelectionContext)
                Renderer::DrawAABB(selection.Mesh->BoundingBox, selection.Entity->GetTransform() * selection.Mesh->Transform);
        }

        Renderer2D::EndScene();
        Renderer::EndRenderPass();
    }

    m_Scene->GetCamera().OnUpdate(ts);
}

void EditorLayer::OnImGuiRender()
{
    static bool pOpen = true;
    static bool fullscreen = true;
    static ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_NoDockingInCentralNode;

    // Main window
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;
    if (fullscreen)
    {
        auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Main Window", &pOpen, windowFlags);
    ImGui::PopStyleVar();

    if (fullscreen)
        ImGui::PopStyleVar(2);

    // Dockspace
    ImGuiIO& io = ImGui::GetIO();
    AB_CORE_ASSERT(io.ConfigFlags & ImGuiConfigFlags_DockingEnable, "Docking must be enabled!");
    ImGuiID dockspaceID = ImGui::GetID("Editor Dockspace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockNodeFlags);

    // ---------------------------- Editor Panel ----------------------------

    // Viewport settings
    ImGui::Begin("Viewport##Settings");
    ImGui::Columns(2);

    Property("Overlay", m_EnableOverlay);

    ImGui::Separator();

    Property("Grid", m_ShowGrid);
    Property("Grid Resolution", m_GridResolution, 0.025f, 0.975f);
    Property("Grid Scale", m_GridScale, 1.0f, 100.0f);

    ImGui::Columns(1);

    char* label = m_SelectionMode == SelectionMode::Entity ? "Entity" : "Mesh";
    if (ImGui::Button(label))
        m_SelectionMode = m_SelectionMode == SelectionMode::Entity ? SelectionMode::Submesh : SelectionMode::Entity;

    ImGui::End();

    // Environment Settings
    ImGui::Begin("Environment");

    if (ImGui::Button("Load Environment Map"))
    {
        std::string filename = FileSystem::OpenFileDialog("HDR:*.hdr", "Select Environment Map");
        if (!filename.empty())
            m_Scene->SetEnvironment(Environment::Load(filename));
    }

    ImGui::Columns(2);
    ImGui::AlignTextToFramePadding();

    Property("Skybox LOD", m_Scene->GetSkyboxLOD(), 0.0f, 11.0f);

    auto& light = m_Scene->GetLight();
    Property("Light Direction", light.Direction);
    Property("Light Radiance", light.Radiance, PropertyFlags::ColorProperty);
    Property("Light Multiplier", light.Multiplier, 0.0f, 10.0f);

    Property("Exposure", m_Scene->GetCamera().GetExposure(), 0.0f, 5.0f);

    ImGui::Columns(1);

    ImGui::End();

    // Editor Panel
    ImGui::Begin("Environment2");

    ImGui::Text("Hi. This is a test window.");

    ImGui::End();

    // Viewport
    ImGui::SetNextWindowDockID(dockspaceID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    auto viewportOffset = ImGui::GetCursorPos();
    auto viewportSize = ImGui::GetContentRegionAvail();
    SceneRenderer::SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
    m_Scene->GetCamera().SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
    m_Scene->GetCamera().SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

    ImGui::Image((void*)SceneRenderer::GetFinalColorBuffer()->GetRendererID(), viewportSize, { 0, 1 }, { 1, 0 });

    auto minBounds = ImGui::GetWindowPos();
    auto windowSize = ImGui::GetWindowSize();

    minBounds.x += viewportOffset.x;
    minBounds.y += viewportOffset.y;
    ImVec2 maxBounds = { minBounds.x + windowSize.x, minBounds.y + windowSize.y };

    m_ViewportBounds[0] = { minBounds.x, minBounds.y };
    m_ViewportBounds[1] = { maxBounds.x, maxBounds.y };
    m_AllowViewportCameraEvents = ImGui::IsMouseHoveringRect(minBounds, maxBounds);

    // Guizmos
    if (m_GizmoMode != -1 && !m_SelectionContext.empty())
    {
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

        bool snap = Input::IsKeyPressed(AB_KEY_LEFT_CONTROL);
        float snapValue[3] = { m_SnapValue, m_SnapValue, m_SnapValue };

        auto& topSelection = m_SelectionContext[0];
        auto& entityTransform = topSelection.Entity->Transform();

        if (m_SelectionMode == SelectionMode::Entity)
        {
            ImGuizmo::Manipulate(
                glm::value_ptr(m_Scene->GetCamera().GetViewMatrix()),
                glm::value_ptr(m_Scene->GetCamera().GetProjectionMatrix()),
                (ImGuizmo::OPERATION)m_GizmoMode,
                ImGuizmo::MODE::LOCAL,
                glm::value_ptr(entityTransform),
                nullptr,
                snap ? snapValue : nullptr);
        }
        else if (m_SelectionMode == SelectionMode::Submesh)
        {
            // TODO: Fix rotation and scaling for multiple selections

            glm::vec3 centroidTranslation(0.0f);
            for (auto& selection : m_SelectionContext)
            {
                float translation[3], rotation[3], scale[3];
                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(selection.Mesh->Transform), translation, rotation, scale);

                glm::vec3 translationVector(translation[0], translation[1], translation[2]);
                centroidTranslation += translationVector;
            }
            centroidTranslation /= (float)m_SelectionContext.size();

            for (uint32_t i = 0; i < m_SelectionContext.size(); i++)
            {
                auto& selection = m_SelectionContext[i];
                if (m_GizmoMode == ImGuizmo::OPERATION::TRANSLATE)
                {
                    glm::mat4 baseTransform = entityTransform * glm::translate(glm::mat4(1.0f), centroidTranslation);
                    glm::mat4 delta;
                    ImGuizmo::Manipulate(
                        glm::value_ptr(m_Scene->GetCamera().GetViewMatrix()),
                        glm::value_ptr(m_Scene->GetCamera().GetProjectionMatrix()),
                        (ImGuizmo::OPERATION)m_GizmoMode,
                        ImGuizmo::MODE::LOCAL,
                        glm::value_ptr(baseTransform),
                        glm::value_ptr(delta),
                        snap ? snapValue : nullptr);

                    selection.Mesh->Transform = glm::inverse(entityTransform) * delta  * entityTransform * selection.Mesh->Transform;
                }
                else
                {
                    glm::mat4 baseTransform = entityTransform * topSelection.Mesh->Transform;
                    glm::mat4 delta;
                    ImGuizmo::Manipulate(
                        glm::value_ptr(m_Scene->GetCamera().GetViewMatrix()),
                        glm::value_ptr(m_Scene->GetCamera().GetProjectionMatrix()),
                        (ImGuizmo::OPERATION)m_GizmoMode,
                        ImGuizmo::MODE::LOCAL,
                        glm::value_ptr(baseTransform),
                        glm::value_ptr(delta),
                        snap ? snapValue : nullptr);

                    selection.Mesh->Transform = glm::inverse(entityTransform) * baseTransform * glm::inverse(topSelection.Mesh->Transform) * selection.Mesh->Transform;
                }
            }
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();

    if (ImGuiDockNode* centralNode = ImGui::DockBuilderGetCentralNode(dockspaceID))
        centralNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit", "Alt+F4"))
                Shutdown();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
    if (e.GetMouseButton() == AB_MOUSE_BUTTON_LEFT && !Input::IsKeyPressed(AB_KEY_LEFT_ALT) && !ImGuizmo::IsOver())
    {
        auto [mouseX, mouseY] = GetMousePosition();
        if (mouseX >= -1.0f && mouseX <= 1.0f && mouseY >= -1.0f && mouseY <= 1.0f)
        {
            if (!Input::IsKeyPressed(AB_KEY_LEFT_SHIFT) && !Input::IsKeyPressed(AB_KEY_RIGHT_SHIFT))
                m_SelectionContext.clear();

            auto [origin, direction] = CastRay(mouseX, mouseY);
            auto mesh = m_MeshEntity->GetMesh();
            auto& submeshes = mesh->GetSubmeshes();

            float minT = std::numeric_limits<float>::max();
            SelectedSubmesh selected(nullptr, nullptr, 0.0f);
            for (uint32_t i = 0; i < submeshes.size(); i++)
            {
                auto& submesh = submeshes[i];
                Ray ray = {
                    glm::inverse(m_MeshEntity->GetTransform() * submesh.Transform) * glm::vec4(origin, 1.0f),
                    glm::inverse(glm::mat3(m_MeshEntity->GetTransform()) * glm::mat3(submesh.Transform)) * direction
                };

                float t;
                if (ray.IntersectsAABB(submesh.BoundingBox, t))
                {
                    const auto& triangleCache = mesh->GetTriangleCache(i);
                    for (const auto& triangle : triangleCache)
                    {
                        if (ray.IntersectsTriangle(triangle.V0.Position, triangle.V1.Position, triangle.V2.Position, t))
                        {
                            if (t < minT)
                            {
                                minT = t;
                                selected = SelectedSubmesh(m_MeshEntity, &submesh, t );
                            }
                            break;
                        }
                    }
                }
            }

            if (selected.Entity)
            {
                if (Input::IsKeyPressed(AB_KEY_LEFT_SHIFT) || Input::IsKeyPressed(AB_KEY_RIGHT_SHIFT))
                {
                    auto pos = std::find_if(m_SelectionContext.begin(), m_SelectionContext.end(), SelectedSubmeshComparator(selected));
                    if (pos != m_SelectionContext.end())
                        m_SelectionContext.erase(pos);
                    else
                        m_SelectionContext.push_back(selected);
                }
                else
                {
                    m_SelectionContext.push_back(selected);
                }
            }
        }
    }

    return false;
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
{
    switch (e.GetKeyCode())
    {
        case AB_KEY_Q:
            m_GizmoMode = -1;
            break;
        case AB_KEY_T:
            m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;
            break;
        case AB_KEY_R:
            m_GizmoMode = ImGuizmo::OPERATION::ROTATE;
            break;
        case AB_KEY_S:
            m_GizmoMode = ImGuizmo::OPERATION::SCALE;
            break;
        case AB_KEY_B:
            if (Input::IsKeyPressed(AB_KEY_LEFT_CONTROL))
                m_EnableOverlay = !m_EnableOverlay;
            break;
        case AB_KEY_G:
            if (Input::IsKeyPressed(AB_KEY_LEFT_CONTROL))
                m_ShowGrid = !m_ShowGrid;
            break;
        case AB_KEY_TAB:
            if (Input::IsKeyPressed(AB_KEY_LEFT_CONTROL))
                m_SelectionMode = m_SelectionMode == SelectionMode::Entity ? SelectionMode::Submesh : SelectionMode::Entity;
            break;
    }
    return false;
}

std::pair<glm::vec3, glm::vec3> EditorLayer::CastRay(float x, float y)
{
    glm::vec3 origin = m_Scene->GetCamera().GetPosition();

    glm::mat4 invProj = glm::inverse(m_Scene->GetCamera().GetProjectionMatrix());
    glm::mat3 invView = glm::inverse(glm::mat3(m_Scene->GetCamera().GetViewMatrix()));

    glm::vec4 mouseClipPos(x, y, -1.0f, 1.0f);
    glm::vec4 ray = invProj * mouseClipPos;
    glm::vec3 direction = invView * glm::vec3(ray);

    return { origin, direction };
}

std::pair<float, float> EditorLayer::GetMousePosition()
{
    auto [x, y] = ImGui::GetMousePos();
    x -= m_ViewportBounds[0].x;
    y -= m_ViewportBounds[0].y;

    float viewportWidth = m_ViewportBounds[1].x - m_ViewportBounds[0].x;
    float viewportHeight = m_ViewportBounds[1].y - m_ViewportBounds[0].y;

    return { (x / viewportWidth) * 2.0f - 1.0f, ((y / viewportHeight) * 2.0f - 1.0f) * -1.0f };
}

void EditorLayer::Property(const std::string& name, bool& value)
{
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    std::string id = "##" + name;
    ImGui::Checkbox(id.c_str(), &value);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

void EditorLayer::Property(const std::string& name, float& value, float min, float max, PropertyFlags flags)
{
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    std::string id = "##" + name;
    ImGui::SliderFloat(id.c_str(), &value, min, max);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

void EditorLayer::Property(const std::string& name, glm::vec2& value, float min, float max, PropertyFlags flags)
{
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    std::string id = "##" + name;
    ImGui::SliderFloat2(id.c_str(), glm::value_ptr(value), min, max);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

void EditorLayer::Property(const std::string& name, glm::vec3& value, float min, float max, PropertyFlags flags)
{
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    std::string id = "##" + name;
    if ((uint32_t)flags & (uint32_t)PropertyFlags::ColorProperty)
        ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
    else
        ImGui::SliderFloat3(id.c_str(), glm::value_ptr(value), min, max);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

void EditorLayer::Property(const std::string& name, glm::vec3& value, PropertyFlags flags)
{
    Property(name, value, -1.0f, 1.0f, flags);
}

void EditorLayer::Property(const std::string& name, glm::vec4& value, float min, float max, PropertyFlags flags)
{
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    std::string id = "##" + name;
    if ((uint32_t)flags & (uint32_t)PropertyFlags::ColorProperty)
        ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
    else
        ImGui::SliderFloat4(id.c_str(), glm::value_ptr(value), min, max);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

void EditorLayer::Property(const std::string& name, glm::vec4& value, PropertyFlags flags)
{
    Property(name, value, -1.0f, 1.0f, flags);
}

}
