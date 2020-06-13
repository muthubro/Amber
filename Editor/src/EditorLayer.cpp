#include "EditorLayer.h"

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

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
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnEvent(Event& e)
{
    //if (m_AllowViewportCameraEvents)
        m_Scene->OnEvent(e);
}

void EditorLayer::OnUpdate(Timestep ts)
{
    Amber::RenderCommand::SetClearColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    Amber::RenderCommand::Clear();

    m_Scene->OnUpdate(ts);

    //if (m_AllowViewportCameraEvents)
        m_Scene->GetCamera().OnUpdate(ts);
}

void EditorLayer::OnImGuiRender()
{
    static bool pOpen = true;
    static bool fullscreen = true;
    static ImGuiDockNodeFlags dockNodeFlags = ImGuiDockNodeFlags_None;

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

    // Editor Panel
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

    // Viewport
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    auto viewportSize = ImGui::GetContentRegionAvail();
    SceneRenderer::SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
    m_Scene->GetCamera().SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
    m_Scene->GetCamera().SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

    ImGui::Image((void*)SceneRenderer::GetFinalColorBuffer()->GetRendererID(), viewportSize, { 0, 1 }, { 1, 0 });

    auto viewportOffset = ImGui::GetCursorPos();
    auto minBounds = ImGui::GetWindowPos();
    auto windowSize = ImGui::GetWindowSize();

    minBounds.x += viewportOffset.x;
    minBounds.y += viewportOffset.y;
    ImVec2 maxBounds = { minBounds.x + windowSize.x, minBounds.y + windowSize.y };
    
    m_ViewportBounds[0] = { minBounds.x, minBounds.y };
    m_ViewportBounds[1] = { maxBounds.x, maxBounds.y };
    m_AllowViewportCameraEvents = ImGui::IsMouseHoveringRect(minBounds, maxBounds);

    ImGui::End();
    ImGui::PopStyleVar();

    // Editor Panel
    ImGui::Begin("Environment2");

    ImGui::Text("Hi. This is a test window.");

    ImGui::End();

    ImGui::End();
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
