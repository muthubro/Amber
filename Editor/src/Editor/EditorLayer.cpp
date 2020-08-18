#include "EditorLayer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <ImGui/imgui.h>
#include <ImGui/imgui_internal.h>

#include <Amber/ImGui/ImGuizmo.h>

#include "Editor/ImGui/ImGuiUtils.h"

namespace Amber
{
namespace Editor
{
// Walls: 64 25 6
// Ground: 18 180 0
// Player: 255 0 0 (Roughness: 0.4, Metalness: 0.0)

static const char* s_ExampleAssembly = "assets/scripts/Terrain.dll";
static std::hash<std::string> s_StringHasher;

static std::tuple<glm::vec3, glm::quat, glm::vec3> GetTransformDecomposition(const glm::mat4& transform)
{
    glm::vec3 translationVector, skew, scaleVector;
    glm::vec4 perspective;
    glm::quat rotationVector;

    glm::decompose(transform, scaleVector, rotationVector, translationVector, skew, perspective);

    return { translationVector, rotationVector, scaleVector };
}

EditorLayer::EditorLayer()
    : Layer("Editor Layer"), m_EditorCamera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 1000.0f))
{
}

void EditorLayer::OnAttach()
{
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
    }

    ScriptEngine::LoadRuntimeAssembly(s_ExampleAssembly);
    m_SceneHierarchyPanel = CreateScope<SceneHierarchyPanel>();

    m_CheckerboardTexture = Texture2D::Create("assets/editor/Checkerboard.png");
    m_PlayButton = Texture2D::Create("assets/editor/PlayButton.png");
    m_PauseButton = Texture2D::Create("assets/editor/PauseButton.png");

    m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;

    NewScene();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnEvent(Event& e)
{
    if (m_AllowViewportCameraEvents)
        m_EditorCamera.OnEvent(e);

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseButtonPressedEvent>(AB_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    dispatcher.Dispatch<KeyPressedEvent>(AB_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
}

void EditorLayer::OnUpdate(Timestep ts)
{
    RenderCommand::SetClearColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    RenderCommand::Clear();

    auto& options = SceneRenderer::GetOptions();
    options.ShowGrid = m_EnableOverlay && m_ShowGrid;
    options.GridResolution = m_GridResolution;
    options.GridScale = m_GridScale;
    options.GridSize = m_GridSize;
    options.ShowBoundingBoxes = m_EnableOverlay && m_ShowBoundingBoxes;
    options.ShowCamera = m_EnableOverlay;

    switch (m_SceneState)
    {
        case SceneState::Edit:
        {
            m_EditorCamera.OnUpdate(ts);

            std::vector<Entity> selectionContext;
            if (m_EnableOverlay)
            {
                for (auto& selection : m_SelectionContext)
                    selectionContext.push_back(selection.Entity);
            }
            m_EditorScene->OnRenderEditor(ts, m_EditorCamera, selectionContext);

            if (m_EnableOverlay)
            {
                Renderer::BeginRenderPass(SceneRenderer::GetFinalRenderPass(), false);
                for (auto& entity : selectionContext)
                {
                    auto boxCollider2D = entity.GetComponentIfExists<BoxCollider2DComponent>();
                    if (boxCollider2D)
                    {
                        const auto& size = boxCollider2D->Size;
                        auto [translation, rotationQuat, scale] = Math::DecomposeTransform(entity.GetTransform());
                        glm::vec3 rotation = glm::eulerAngles(rotationQuat);

                        Renderer2D::BeginScene(m_EditorCamera.GetViewProjection(), false);
                        Renderer2D::DrawQuad({ translation, rotation.z, size * 2.0f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f) });
                        Renderer2D::EndScene();
                    }

                    auto circleCollider2D = entity.GetComponentIfExists<CircleCollider2DComponent>();
                    if (circleCollider2D)
                    {
                        float radius = circleCollider2D->Radius;
                        auto [translation, rotationQuat, scale] = Math::DecomposeTransform(entity.GetTransform());
                        glm::vec3 rotation = glm::eulerAngles(rotationQuat);

                        Renderer2D::BeginScene(m_EditorCamera.GetViewProjection(), false);
                        Renderer2D::DrawQuad({ translation, rotation.z, glm::vec2(radius * 2.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f) });
                        Renderer2D::EndScene();
                    }
                }
                Renderer::EndRenderPass();
            }

            break;
        }

        case SceneState::Play:
            m_RuntimeScene->OnUpdate(ts);
            m_RuntimeScene->OnRenderRuntime(ts);
            break;

        case SceneState::Pause:
            break;
    }
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
    m_DockspaceID = ImGui::GetID("Editor Dockspace");
    ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), dockNodeFlags);

    ImGui::AlignTextToFramePadding();

    // ---------------------------- Editor Panel ----------------------------
    // Editor settings
    DrawViewportSettingsPanel();
    DrawEnvironmentSettingsPanel();

    // Model settings
    DrawAnimationPanel();
    DrawMaterialsPanel();

    DrawViewport();

    DrawToolbar();
    DrawMenuBar();

    m_SceneHierarchyPanel->OnImGuiRender();
    ScriptEngine::OnImGuiRender();

    ImGui::End();
}

void EditorLayer::UpdateWindowTitle(const std::string& sceneName)
{
    std::string title = sceneName + " - Amber Editor (" + Application::GetConfiguration() + ")";
    Application::Get().GetWindow().SetTitle(title);
}

void EditorLayer::DrawViewportSettingsPanel()
{
    ImGui::Begin("Viewport##Settings");

    BeginPropertyGrid();

    Property("Overlay", m_EnableOverlay);
    ImGui::Separator();
    Property("Grid", m_ShowGrid);
    Property("Grid Resolution", m_GridResolution, 0.025f, 0.975f);
    Property("Grid Scale", m_GridScale, 1.0f, 100.0f);
    Property("Grid Size", m_GridSize, 1.0f, 100.0f);
    Property("Bounding Box", m_ShowBoundingBoxes);

    EndPropertyGrid();

    char* label = m_SelectionMode == SelectionMode::Entity ? "Entity" : "Mesh";
    if (ImGui::Button(label))
        m_SelectionMode = m_SelectionMode == SelectionMode::Entity ? SelectionMode::Submesh : SelectionMode::Entity;

    ImGui::End();
}

void EditorLayer::DrawEnvironmentSettingsPanel()
{
    ImGui::Begin("Environment");

    auto& environment = m_EditorScene->GetEnvironment();
    
    BeginPropertyGrid(2, (int)s_StringHasher("environmentMap"));
    Property("", environment.Filepath.empty() ? "NULL" : environment.Filepath.c_str());
    if (ImGui::Button("..."))
    {
        std::string filename = FileSystem::OpenFileDialog("HDR (*.hdr):*.hdr", "Select Environment Map");
        if (!filename.empty())
        {
            m_EditorScene->SetEnvironment(Environment::Load(filename));
            m_EditorScene->SetLight({ glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f), 1.0f });
        }
    }
    EndPropertyGrid();

    ImGui::Separator();

    BeginPropertyGrid(2, (int)s_StringHasher("light"));
    if (!environment.Filepath.empty())
    {
        Property("Environment Rotation", environment.Rotation, -360.0f, 360.0f);
        Property("Skybox LOD", m_EditorScene->GetSkyboxLOD(), 0.0f, 11.0f);
        Property("Exposure", m_EditorCamera.Exposure(), 0.0f, 5.0f);

        ImGui::Separator();

        auto& light = m_EditorScene->GetLight();
        glm::vec3 direction = light.Direction;
        Property("Light Direction", direction);
        Property("Light Radiance", light.Radiance, PropertyFlags::ColorProperty);
        Property("Light Multiplier", light.Multiplier, 0.0f, 10.0f);
        light.Direction = glm::normalize(direction);
    }
    EndPropertyGrid();

    ImGui::Separator();

    BeginPropertyGrid(2, (int)s_StringHasher("gravity"));
    float gravity = -1.0f * m_EditorScene->GetPhysics2DGravity();
    if (Property("Gravity", gravity, -100.0f, 100.0f, 1.0f))
        m_EditorScene->SetPhysics2DGravity(-1.0f * gravity);
    EndPropertyGrid();

    ImGui::End();
}

void EditorLayer::DrawAnimationPanel()
{
    ImGui::Begin("Animation");

    if (!m_SelectionContext.empty())
    {
        auto& topSelection = *m_SelectionContext.rbegin();
        auto mesh = topSelection.Entity.GetComponentIfExists<MeshComponent>();
        if (mesh && *mesh && mesh->Mesh->IsAnimated())
        {
            BeginPropertyGrid();

            Property("Toggle Animation", mesh->Mesh->IsAnimationPlaying());
            Property("Animation Time", mesh->Mesh->AnimationTimeMultiplier(), 0.1f, 10.0f, 0.1f);

            EndPropertyGrid();
        }
    }

    ImGui::End();
}

void EditorLayer::DrawMaterialsPanel()
{
    static std::array<const char*, (size_t)ShaderType::Count - 1> shaders = {
        "Standard Static",
        "Standard Animated",
        "Unlit - Color",
        "Unlit - Texture"
    };

    ImGui::Begin("Materials");

    if (!m_SelectionContext.empty())
    {
        auto& topSelection = *m_SelectionContext.rbegin();
        auto mesh = topSelection.Entity.GetComponentIfExists<MeshComponent>();
        if (topSelection.Mesh && mesh)
        {
            auto baseMaterial = mesh->Mesh->GetMaterial();

            auto currentShader = baseMaterial->GetShader()->GetType();
            uint8_t shaderIndex = (uint8_t)currentShader - 1;

            ImGui::Text("Shader");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##ShaderList", shaders[shaderIndex]))
            {
                for (uint8_t i = 0; i < shaders.size(); i++)
                {
                    bool selected = i == shaderIndex;
                    if (ImGui::Selectable(shaders[i], &selected))
                    {
                        if (i != shaderIndex)
                        {
                            baseMaterial->Reset(Renderer::GetShaderLibrary()->Get((ShaderType)(i + 1)));

                            switch (i)
                            {
                                case 2:
                                    mesh->Mesh->SetAlbedo(*topSelection.Mesh, glm::vec3(1.0f));
                                    break;

                                case 3:
                                    mesh->Mesh->SetAlbedoTexture(*topSelection.Mesh, true, Renderer2D::WhiteTexture());
                                    break;
                            }
                        }
                    }
                }
                if (ImGui::Selectable("Custom..."))
                {
                    std::string filepath = FileSystem::OpenFileDialog("GLSL (*.glsl):*.glsl", "Select Shader");
                    if (!filepath.empty())
                        baseMaterial->Reset(Shader::Create(filepath));
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();

            auto material = mesh->Mesh->GetMaterials()[topSelection.Mesh->MaterialIndex];
            Property("Material", material->GetName());

            switch (baseMaterial->GetShader()->GetType())
            {
                case ShaderType::StandardStatic:
                case ShaderType::StandardAnimated:
                {
                    // Albedo
                    if (ImGui::CollapsingHeader("Albedo", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        bool hasAlbedo = topSelection.Mesh->HasAlbedoMap;
                        bool usingAlbedoMap = mesh->Mesh->UsingAlbedoTexture(*topSelection.Mesh);
                        auto albedoMap = mesh->Mesh->GetAlbedoTexture(*topSelection.Mesh);
                        glm::vec3 albedo = mesh->Mesh->GetAlbedo(*topSelection.Mesh);

                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
                        ImGui::Image((void*)(size_t)(hasAlbedo ? albedoMap->GetRendererID() : m_CheckerboardTexture->GetRendererID()),
                                     ImVec2(64, 64));
                        ImGui::PopStyleVar();

                        if (ImGui::IsItemHovered())
                        {
                            if (hasAlbedo)
                            {
                                ImGui::BeginTooltip();
                                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                                ImGui::TextUnformatted(albedoMap->GetAssetPath().c_str());
                                ImGui::PopTextWrapPos();
                                ImGui::Image((void*)(size_t)albedoMap->GetRendererID(), ImVec2(384, 384));
                                ImGui::EndTooltip();
                            }

                            if (ImGui::IsItemClicked())
                            {
                                std::string filename = FileSystem::OpenFileDialog("", "Select Albedo Texture");
                                if (!filename.empty())
                                {
                                    mesh->Mesh->SetAlbedoTexture(*topSelection.Mesh, usingAlbedoMap, Texture2D::Create(filename, true));
                                    usingAlbedoMap = true;
                                }
                            }
                        }

                        ImGui::SameLine();

                        ImGui::BeginGroup();
                        BeginPropertyGrid(2, (int)s_StringHasher("albedo"));
                        Property("Use Albedo Texture", usingAlbedoMap);
                        Property("Albedo Color", albedo, PropertyFlags::ColorProperty);
                        EndPropertyGrid();
                        ImGui::EndGroup();

                        mesh->Mesh->SetAlbedoTexture(*topSelection.Mesh, usingAlbedoMap);
                        mesh->Mesh->SetAlbedo(*topSelection.Mesh, albedo);
                    }

                    // Normal
                    if (ImGui::CollapsingHeader("Normal", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        bool hasNormal = topSelection.Mesh->HasNormalMap;
                        bool usingNormalMap = mesh->Mesh->UsingNormalTexture(*topSelection.Mesh);
                        auto normalMap = mesh->Mesh->GetNormalTexture(*topSelection.Mesh);

                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
                        ImGui::Image((void*)(size_t)(hasNormal ? normalMap->GetRendererID() : m_CheckerboardTexture->GetRendererID()),
                                     ImVec2(64, 64));
                        ImGui::PopStyleVar();

                        if (ImGui::IsItemHovered())
                        {
                            if (hasNormal)
                            {
                                ImGui::BeginTooltip();
                                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                                ImGui::TextUnformatted(normalMap->GetAssetPath().c_str());
                                ImGui::PopTextWrapPos();
                                ImGui::Image((void*)(size_t)normalMap->GetRendererID(), ImVec2(384, 384));
                                ImGui::EndTooltip();
                            }

                            if (ImGui::IsItemClicked())
                            {
                                std::string filename = FileSystem::OpenFileDialog("", "Select Normal Texture");
                                if (!filename.empty())
                                {
                                    mesh->Mesh->SetNormalTexture(*topSelection.Mesh, true, Texture2D::Create(filename));
                                    usingNormalMap = true;
                                }
                            }
                        }

                        ImGui::SameLine();

                        ImGui::BeginGroup();
                        BeginPropertyGrid(2, (int)s_StringHasher("normal"));
                        Property("Use Normal Texture", usingNormalMap);
                        EndPropertyGrid();
                        ImGui::EndGroup();

                        mesh->Mesh->SetNormalTexture(*topSelection.Mesh, usingNormalMap);
                    }

                    // Roughness
                    if (ImGui::CollapsingHeader("Roughness", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        bool hasRoughness = topSelection.Mesh->HasRoughnessMap;
                        bool usingRoughnessMap = mesh->Mesh->UsingRoughnessTexture(*topSelection.Mesh);
                        auto roughnessMap = mesh->Mesh->GetRoughnessTexture(*topSelection.Mesh);
                        float roughness = mesh->Mesh->GetRoughness(*topSelection.Mesh);

                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
                        ImGui::Image((void*)(size_t)(hasRoughness ? roughnessMap->GetRendererID() : m_CheckerboardTexture->GetRendererID()),
                                     ImVec2(64, 64));
                        ImGui::PopStyleVar();

                        if (ImGui::IsItemHovered())
                        {
                            if (hasRoughness)
                            {
                                ImGui::BeginTooltip();
                                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                                ImGui::TextUnformatted(roughnessMap->GetAssetPath().c_str());
                                ImGui::PopTextWrapPos();
                                ImGui::Image((void*)(size_t)roughnessMap->GetRendererID(), ImVec2(384, 384));
                                ImGui::EndTooltip();
                            }

                            if (ImGui::IsItemClicked())
                            {
                                std::string filename = FileSystem::OpenFileDialog("", "Select Roughness Texture");
                                if (!filename.empty())
                                {
                                    mesh->Mesh->SetRoughnessTexture(*topSelection.Mesh, true, Texture2D::Create(filename));
                                    usingRoughnessMap = true;
                                }
                            }
                        }

                        ImGui::SameLine();

                        ImGui::BeginGroup();
                        BeginPropertyGrid(2, (int)s_StringHasher("roughness"));
                        Property("Use Roughness Texture", usingRoughnessMap);
                        Property("Roughness", roughness, 0.0f, 1.0f, 0.01f);
                        EndPropertyGrid();
                        ImGui::EndGroup();

                        mesh->Mesh->SetRoughnessTexture(*topSelection.Mesh, usingRoughnessMap);
                        mesh->Mesh->SetRoughness(*topSelection.Mesh, roughness);
                    }

                    // Metalness
                    if (ImGui::CollapsingHeader("Metalness", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        bool hasMetalness = topSelection.Mesh->HasMetalnessMap;
                        bool usingMetalnessMap = mesh->Mesh->UsingMetalnessTexture(*topSelection.Mesh);
                        auto metalnessMap = mesh->Mesh->GetMetalnessTexture(*topSelection.Mesh);
                        float metalness = mesh->Mesh->GetMetalness(*topSelection.Mesh);

                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
                        ImGui::Image((void*)(size_t)(hasMetalness ? metalnessMap->GetRendererID() : m_CheckerboardTexture->GetRendererID()),
                                     ImVec2(64, 64));
                        ImGui::PopStyleVar();

                        if (ImGui::IsItemHovered())
                        {
                            if (hasMetalness)
                            {
                                ImGui::BeginTooltip();
                                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                                ImGui::TextUnformatted(metalnessMap->GetAssetPath().c_str());
                                ImGui::PopTextWrapPos();
                                ImGui::Image((void*)(size_t)metalnessMap->GetRendererID(), ImVec2(384, 384));
                                ImGui::EndTooltip();
                            }

                            if (ImGui::IsItemClicked())
                            {
                                std::string filename = FileSystem::OpenFileDialog("", "Select Metalness Texture");
                                if (!filename.empty())
                                {
                                    mesh->Mesh->SetMetalnessTexture(*topSelection.Mesh, true, Texture2D::Create(filename));
                                    usingMetalnessMap = true;
                                }
                            }
                        }

                        ImGui::SameLine();

                        ImGui::BeginGroup();
                        BeginPropertyGrid(2, (int)s_StringHasher("metalness"));
                        Property("Use Metalness Texture", usingMetalnessMap);
                        Property("Metalness", metalness, 0.0f, 1.0f, 0.01f);
                        EndPropertyGrid();
                        ImGui::EndGroup();

                        mesh->Mesh->SetMetalnessTexture(*topSelection.Mesh, usingMetalnessMap);
                        mesh->Mesh->SetMetalness(*topSelection.Mesh, metalness);
                    }

                    break;
                }

                case ShaderType::UnlitColor:
                {
                    glm::vec3 color = mesh->Mesh->GetAlbedo(*topSelection.Mesh);
                    if (Property("Color", color, PropertyFlags::ColorProperty))
                        mesh->Mesh->SetAlbedo(*topSelection.Mesh, color);
                    
                    break;
                }

                case ShaderType::UnlitTexture:
                {
                    bool hasTexture = topSelection.Mesh->HasAlbedoMap;
                    auto texture = mesh->Mesh->GetAlbedoTexture(*topSelection.Mesh);

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
                    ImGui::Image((void*)(size_t)(hasTexture ? texture->GetRendererID() : m_CheckerboardTexture->GetRendererID()),
                                 ImVec2(64, 64));
                    ImGui::PopStyleVar();

                    if (ImGui::IsItemHovered())
                    {
                        if (hasTexture)
                        {
                            ImGui::BeginTooltip();
                            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                            ImGui::TextUnformatted(texture->GetAssetPath().c_str());
                            ImGui::PopTextWrapPos();
                            ImGui::Image((void*)(size_t)texture->GetRendererID(), ImVec2(384, 384));
                            ImGui::EndTooltip();
                        }

                        if (ImGui::IsItemClicked())
                        {
                            std::string filename = FileSystem::OpenFileDialog("", "Select Texture");
                            if (!filename.empty())
                                mesh->Mesh->SetAlbedoTexture(*topSelection.Mesh, true, Texture2D::Create(filename));
                        }
                    }

                    ImGui::SameLine();
                    ImGui::Text("Texture");
                    
                    break;
                }
            }
        }
    }

    ImGui::End();
}

void EditorLayer::DrawViewport()
{
    ImGui::SetNextWindowDockID(m_DockspaceID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");

    m_ViewportPanelFocused = ImGui::IsWindowFocused();
    m_ViewportPanelMouseOver = ImGui::IsWindowHovered();

    auto viewportOffset = ImGui::GetCursorPos();
    auto viewportSize = ImGui::GetContentRegionAvail();

    m_EditorCamera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
    
    SceneRenderer::SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
    m_EditorScene->SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
    m_EditorCamera.SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
    auto sceneCameras = m_EditorScene->GetAllEntitiesWith<CameraComponent>();
    for (auto& cameraEntity : sceneCameras)
    {
        Entity entity{ cameraEntity, m_EditorScene.Raw() };
        entity.GetComponent<CameraComponent>().Camera.SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
    }
    if (m_RuntimeScene)
        m_RuntimeScene->SetViewportSize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);

    ImGui::Image((void*)(size_t)SceneRenderer::GetFinalColorBuffer()->GetRendererID(), viewportSize, { 0, 1 }, { 1, 0 });

    auto minBounds = ImGui::GetWindowPos();
    auto windowSize = ImGui::GetWindowSize();

    minBounds.x += viewportOffset.x;
    minBounds.y += viewportOffset.y;
    ImVec2 maxBounds = { minBounds.x + windowSize.x, minBounds.y + windowSize.y };

    m_ViewportBounds[0] = { minBounds.x, minBounds.y };
    m_ViewportBounds[1] = { maxBounds.x, maxBounds.y };
    m_AllowViewportCameraEvents = ImGui::IsMouseHoveringRect(minBounds, maxBounds);

    // Gizmos
    if (m_GizmoMode != -1 && !m_SelectionContext.empty() && m_EnableOverlay)
    {
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

        bool snap = Input::IsKeyPressed(AB_KEY_LEFT_CONTROL);
        float snapValue[3] = { m_SnapValue, m_SnapValue, m_SnapValue };

        // TODO: Multi-selection scaling
        if (m_GizmoMode == ImGuizmo::OPERATION::SCALE)
        {
            auto& selection = *m_SelectionContext.rbegin();
            auto& entityTransform = selection.Entity.Transform();
            if (m_SelectionMode == SelectionMode::Entity)
            {
                ImGuizmo::Manipulate(
                    glm::value_ptr(m_EditorCamera.GetViewMatrix()),
                    glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),
                    (ImGuizmo::OPERATION)m_GizmoMode,
                    ImGuizmo::MODE::LOCAL,
                    glm::value_ptr(entityTransform),
                    nullptr,
                    snap ? snapValue : nullptr);
            }
            else
            {
                auto transform = entityTransform * selection.Mesh->Transform;
                ImGuizmo::Manipulate(
                    glm::value_ptr(m_EditorCamera.GetViewMatrix()),
                    glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),
                    (ImGuizmo::OPERATION)m_GizmoMode,
                    ImGuizmo::MODE::LOCAL,
                    glm::value_ptr(transform),
                    nullptr,
                    snap ? snapValue : nullptr);

                selection.Mesh->Transform = glm::inverse(entityTransform) * transform;
            }
        }
        else
        {
            glm::vec3 centroidTranslation(0.0f);
            for (auto& selection : m_SelectionContext)
            {
                float translation[3], rotation[3], scale[3];
                if (m_SelectionMode == SelectionMode::Entity)
                    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(selection.Entity.Transform()), translation, rotation, scale);
                else
                    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(selection.Entity.Transform() * selection.Mesh->Transform), translation, rotation, scale);

                centroidTranslation.x += translation[0];
                centroidTranslation.y += translation[1];
                centroidTranslation.z += translation[2];
            }
            centroidTranslation /= (float)m_SelectionContext.size();
            glm::mat4 centroidTransform = glm::translate(glm::mat4(1.0f), centroidTranslation);

            glm::mat4 delta;
            ImGuizmo::Manipulate(
                glm::value_ptr(m_EditorCamera.GetViewMatrix()),
                glm::value_ptr(m_EditorCamera.GetProjectionMatrix()),
                (ImGuizmo::OPERATION)m_GizmoMode,
                ImGuizmo::MODE::LOCAL,
                glm::value_ptr(centroidTransform),
                glm::value_ptr(delta),
                snap ? snapValue : nullptr);

            for (uint32_t i = 0; i < m_SelectionContext.size(); i++)
            {
                auto& selection = m_SelectionContext[i];
                auto& entityTransform = selection.Entity.Transform();

                if (m_SelectionMode == SelectionMode::Entity)
                {
                    entityTransform = delta * entityTransform;
                    if (m_GizmoMode == ImGuizmo::OPERATION::ROTATE && ImGuizmo::IsUsing())
                    {
                        auto [translation, rotation, scale] = GetTransformDecomposition(delta);
                        m_SceneHierarchyPanel->SetOrientation(selection.Entity, rotation);
                    }
                }
                else
                {
                    if (!selection.Entity.HasComponent<MeshComponent>())
                        continue;
                    selection.Mesh->Transform = glm::inverse(entityTransform) * delta * entityTransform * selection.Mesh->Transform;
                }
            }
        }
    }

    ImGui::End();
    ImGui::PopStyleVar();

    if (ImGuiDockNode* centralNode = ImGui::DockBuilderGetCentralNode(m_DockspaceID))
        centralNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
}

void EditorLayer::DrawToolbar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12.0f, 6.0f));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::Begin("Toolbar");

    switch (m_SceneState)
    {
        case SceneState::Edit:
            if (ImGui::ImageButton((ImTextureID)(size_t)(m_PlayButton->GetRendererID()), ImVec2(32.0f, 32.0f)))
                OnScenePlay();
            break;

        case SceneState::Play:
            if (ImGui::ImageButton((ImTextureID)(size_t)(m_PauseButton->GetRendererID()), ImVec2(32.0f, 32.0f)))
                OnSceneStop();
            break;
    }
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);

    ImGui::End();
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
}

void EditorLayer::DrawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene...", "Ctrl+N"))
                NewScene();

            if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
                OpenScene();

            ImGui::Separator();

            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
                SaveScene(m_EditorScene->GetAssetPath());

            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S"))
                SaveScene();

            ImGui::Separator();

            if (ImGui::MenuItem("Reload C# Assembly"))
                ScriptEngine::ReloadAssembly("assets/scripts/Terrain.dll");

            ImGui::MenuItem("Reload Assembly On Play", nullptr, &m_ReloadScriptOnPlay);

            ImGui::Separator();

            if (ImGui::MenuItem("Exit", "Alt+F4"))
                Shutdown();

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void EditorLayer::NewScene()
{
    m_EditorScene = Ref<Scene>::Create("Untitled Scene");
    UpdateWindowTitle(m_EditorScene->GetDebugName());

    auto cameraEntity = m_EditorScene->CreateEntity("Camera");
    auto& camera = cameraEntity.AddComponent<CameraComponent>();
    auto [width, height] = m_EditorCamera.GetViewportSize();
    camera.Camera.SetViewportSize(width, height);

    ScriptEngine::SetSceneContext(m_EditorScene);
    m_SceneHierarchyPanel->SetContext(m_EditorScene);

    m_SelectionContext.clear();
    m_SceneHierarchyPanel->SetSelection(m_SelectionContext);
}

void EditorLayer::OpenScene()
{
    std::string filepath = FileSystem::OpenFileDialog("Amber Scene (*.asc):*.asc", "Open Scene");
    if (!filepath.empty())
    {
        Ref<Scene> scene = Ref<Scene>::Create("", filepath);
        SceneSerializer serializer(scene);
        serializer.Deserialize(filepath);

        m_EditorScene = scene;
        UpdateWindowTitle(m_EditorScene->GetDebugName());
        m_SceneHierarchyPanel->SetContext(scene);
        ScriptEngine::SetSceneContext(scene);
        m_SelectionContext.clear();
    }
}

void EditorLayer::SaveScene(const std::string& path)
{
    std::string filepath = path.empty() ? FileSystem::SaveFileDialog("Amber Scene (*.asc):*.asc", "Save Scene") : path;
    if (!filepath.empty())
    {
        std::string sceneName;

        auto first = filepath.find_last_of("/\\");
        if (first == std::string::npos)
            first = 0;

        auto last = filepath.rfind('.');
        if (last == std::string::npos)
        {
            sceneName = filepath.substr(first + 1);
            filepath += ".asc";
        }
        else
        {
            auto extension = filepath.substr(last + 1);
            if (extension != "asc")
            {
                filepath += ".asc";
                last = filepath.size() - 4;
            }

            sceneName = filepath.substr(first + 1, last - first - 1);
        }

        m_EditorScene->SetDebugName(sceneName);
        SceneSerializer serializer(m_EditorScene);
        serializer.Serialize(filepath);
        m_EditorScene->SetAssetPath(filepath);

        UpdateWindowTitle(sceneName);
    }
}

void EditorLayer::OnScenePlay()
{
    m_SelectionContext.clear();
    m_SceneState = SceneState::Play;

    if (m_ReloadScriptOnPlay)
        ScriptEngine::ReloadAssembly(s_ExampleAssembly);

    m_RuntimeScene = Ref<Scene>::Create();
    m_EditorScene->CopyTo(m_RuntimeScene);

    m_RuntimeScene->OnRuntimeStart();
    m_SceneHierarchyPanel->SetContext(m_RuntimeScene);

    m_EnableOverlay = false;
}

void EditorLayer::OnSceneStop()
{
    m_RuntimeScene->OnRuntimeStop();
    m_SceneState = SceneState::Edit;

    m_RuntimeScene = nullptr;

    m_SelectionContext.clear();
    ScriptEngine::SetSceneContext(m_EditorScene);
    m_SceneHierarchyPanel->SetContext(m_EditorScene);
 
    m_EnableOverlay = true;
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
    if (e.GetMouseButton() == AB_MOUSE_BUTTON_LEFT && !Input::IsKeyPressed(AB_KEY_LEFT_ALT) && !ImGuizmo::IsOver())
    {
        auto [mouseX, mouseY] = GetMousePosition();
        if (mouseX >= -1.0f && mouseX <= 1.0f && mouseY >= -1.0f && mouseY <= 1.0f)
        {
            bool changed = false;
            if (!Input::IsKeyPressed(AB_KEY_LEFT_SHIFT) && !Input::IsKeyPressed(AB_KEY_RIGHT_SHIFT) &&
                !Input::IsKeyPressed(AB_KEY_LEFT_CONTROL) && !Input::IsKeyPressed(AB_KEY_RIGHT_CONTROL))
            {
                changed = !m_SelectionContext.empty();
                m_SelectionContext.clear();
            }

            auto [origin, direction] = CastRay(mouseX, mouseY);
            auto entities = m_EditorScene->GetAllEntitiesWith<MeshComponent, TransformComponent>();

            float minT = std::numeric_limits<float>::max();
            SelectedSubmesh selected(Entity(), nullptr, 0.0f);
            for (auto entity : entities)
            {
                Ref<Mesh> mesh = entities.get<MeshComponent>(entity);
                auto& submeshes = mesh->GetSubmeshes();

                for (uint32_t i = 0; i < submeshes.size(); i++)
                {
                    auto& submesh = submeshes[i];
                    const glm::mat4& transform = entities.get<TransformComponent>(entity);
                    Math::Ray ray = {
                        glm::inverse(transform * submesh.Transform) * glm::vec4(origin, 1.0f),
                        glm::inverse(glm::mat3(transform) * glm::mat3(submesh.Transform)) * direction
                    };

                    float t;
                    if (ray.IntersectsAABB(submesh.BoundingBox, t))
                    {
                        Entity selectedEntity(entity, m_EditorScene.Raw());

                        // TODO: Triangle check for animated meshes?
                        if (mesh->IsAnimated())
                        {
                            if (t < minT)
                            {
                                minT = t;
                                selected = SelectedSubmesh(selectedEntity, &submesh, t);
                            }
                        }
                        else
                        {
                            const auto& triangleCache = mesh->GetTriangleCache(i);
                            for (const auto& triangle : triangleCache)
                            {
                                if (ray.IntersectsTriangle(triangle.V0, triangle.V1, triangle.V2, t))
                                {
                                    if (t < minT)
                                    {
                                        minT = t;
                                        selected = SelectedSubmesh(selectedEntity, &submesh, t);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if (selected.Mesh)
            {
                if (Input::IsKeyPressed(AB_KEY_LEFT_SHIFT) || Input::IsKeyPressed(AB_KEY_RIGHT_SHIFT) ||
                    Input::IsKeyPressed(AB_KEY_LEFT_CONTROL) || Input::IsKeyPressed(AB_KEY_RIGHT_CONTROL))
                {
                    std::vector<SelectedSubmesh>::iterator pos;
                    if (m_SelectionMode == SelectionMode::Entity)
                        pos = std::find_if(m_SelectionContext.begin(), m_SelectionContext.end(), SelectedEntityComparator(selected));
                    else
                        pos = std::find_if(m_SelectionContext.begin(), m_SelectionContext.end(), SelectedSubmeshComparator(selected));

                    if (pos != m_SelectionContext.end())
                        m_SelectionContext.erase(pos);
                    else
                        m_SelectionContext.push_back(selected);
                }
                else
                {
                    m_SelectionContext.push_back(selected);
                }
                
                changed = true;
            }
            if (changed)
                m_SceneHierarchyPanel->SetSelection(m_SelectionContext);
        }
    }

    return false;
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
{
    if (m_RuntimeScene && m_RuntimeScene->IsPlaying())
        ImGui::CaptureKeyboardFromApp(false);

    if (m_ViewportPanelFocused)
    {
        bool handled = true;
        switch (e.GetKeyCode())
        {
            case AB_KEY_Q:
                m_GizmoMode = -1;
                break;
            case AB_KEY_W:
                m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE;
                break;
            case AB_KEY_E:
                m_GizmoMode = ImGuizmo::OPERATION::ROTATE;
                break;
            case AB_KEY_R:
                m_GizmoMode = ImGuizmo::OPERATION::SCALE;
                break;

            case AB_KEY_DELETE:
                for (auto& selection : m_SelectionContext)
                    m_EditorScene->DestroyEntity(selection.Entity);
                m_SelectionContext.clear();
                m_SceneHierarchyPanel->SetSelection(m_SelectionContext);
                break;

            default:
                handled = false;
        }

        if (handled)
            return false;
    }

    if (Input::IsKeyPressed(AB_KEY_LEFT_CONTROL) || Input::IsKeyPressed(AB_KEY_RIGHT_CONTROL))
    {
        if (Input::IsKeyPressed(AB_KEY_LEFT_SHIFT) || Input::IsKeyPressed(AB_KEY_RIGHT_SHIFT))
        {
            bool handled = true;
            switch (e.GetKeyCode())
            {
                case AB_KEY_O:
                    m_EnableOverlay = !m_EnableOverlay;
                    break;

                default:
                    handled = false;
            }
            if (handled)
                return false;
        }

        bool handled = true;
        switch (e.GetKeyCode())
        {
            case AB_KEY_B:
                m_ShowBoundingBoxes = !m_ShowBoundingBoxes;
                break;
            case AB_KEY_G:
                m_ShowGrid = !m_ShowGrid;
                break;
            case AB_KEY_TAB:
                m_SelectionMode = m_SelectionMode == SelectionMode::Entity ? SelectionMode::Submesh : SelectionMode::Entity;
                break;

            case AB_KEY_D:
                for (auto& selection : m_SelectionContext)
                    m_EditorScene->DuplicateEntity(selection.Entity);
                break;

            // Menu shortcuts
            case AB_KEY_N:
                NewScene();
                break;
            case AB_KEY_O:
                OpenScene();
                break;
            case AB_KEY_S:
                if (Input::IsKeyPressed(AB_KEY_LEFT_SHIFT) || Input::IsKeyPressed(AB_KEY_RIGHT_SHIFT))
                    SaveScene();
                else
                    SaveScene(m_EditorScene->GetAssetPath());
                break;

            default:
                handled = false;
        }
        if (handled)
            return false;
    }

    return false;
}

std::pair<glm::vec3, glm::vec3> EditorLayer::CastRay(float x, float y)
{
    glm::vec3 origin = m_EditorCamera.GetPosition();

    glm::mat4 invProj = glm::inverse(m_EditorCamera.GetProjectionMatrix());
    glm::mat3 invView = glm::inverse(glm::mat3(m_EditorCamera.GetViewMatrix()));

    glm::vec4 mouseClipPos(x, y, -1.0f, 1.0f);
    glm::vec4 ray = invProj * mouseClipPos;
    glm::vec3 direction = invView * glm::vec3(ray);

    return { origin, direction };
}

std::pair<float, float> EditorLayer::GetMousePosition()
{
    auto [x, y] = Input::GetMousePosition();
    x -= m_ViewportBounds[0].x;
    y -= m_ViewportBounds[0].y;

    float viewportWidth = m_ViewportBounds[1].x - m_ViewportBounds[0].x;
    float viewportHeight = m_ViewportBounds[1].y - m_ViewportBounds[0].y;

    return { (x / viewportWidth) * 2.0f - 1.0f, ((y / viewportHeight) * 2.0f - 1.0f) * -1.0f };
}

} // Editor
} // Amber
