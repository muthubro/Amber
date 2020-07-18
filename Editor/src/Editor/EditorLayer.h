#pragma once

#include <vector>

#include <Amber.h>

#include "Editor/ScenePanel/SceneHierarchyPanel.h"

namespace Amber
{
namespace Editor
{

struct SelectedSubmesh
{
    Entity Entity;
    Submesh* Mesh;
    float Distance;

    SelectedSubmesh() = default;
    SelectedSubmesh(Amber::Entity entity, Submesh* mesh, float distance)
        : Entity(entity), Mesh(mesh), Distance(distance)
    {
    }
};

struct SelectedSubmeshComparator
{
    explicit SelectedSubmeshComparator(const SelectedSubmesh& mesh) : SelectedMesh(mesh) {}

    bool operator() (const SelectedSubmesh& other) const
    {
        return SelectedMesh.Mesh == other.Mesh;
    }

    SelectedSubmesh SelectedMesh;
};

struct SelectedEntityComparator
{
    explicit SelectedEntityComparator(const SelectedSubmesh& mesh) : SelectedMesh(mesh) {}

    bool operator() (const SelectedSubmesh& other) const
    {
        return (Entity)SelectedMesh.Entity == other.Entity;
    }

    SelectedSubmesh SelectedMesh;
};

class EditorLayer : public Layer
{
public:
    EditorLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Event& e) override;
    void OnUpdate(Timestep ts) override;
    void OnImGuiRender() override;

private:
    enum class SelectionMode
    {
        None = 0, Entity, Submesh
    };

    enum class SceneState
    {
        Edit = 0, Play, Pause
    };

    Ref<Scene> m_EditorScene, m_RuntimeScene;
    EditorCamera m_EditorCamera;

    Ref<Texture2D> m_CheckerboardTexture;
    Ref<Texture2D> m_PlayButton, m_PauseButton;

    SceneState m_SceneState = SceneState::Edit;

    uint32_t m_DockspaceID;

    glm::vec2 m_ViewportBounds[2];
    bool m_AllowViewportCameraEvents = false;
    bool m_ViewportPanelMouseOver = false;
    bool m_ViewportPanelFocused = false;

    bool m_EnableOverlay = true;
    bool m_ShowGrid = true;
    float m_GridResolution = 0.025f;
    float m_GridScale = 16.025f;
    float m_GridSize = 16.025f;
    bool m_ShowBoundingBoxes = false;

    std::vector<SelectedSubmesh> m_SelectionContext;
    float m_SnapValue = 0.5f;
    int m_GizmoMode = -1;
    SelectionMode m_SelectionMode = SelectionMode::Entity;

    Scope<SceneHierarchyPanel> m_SceneHierarchyPanel = nullptr;

    void UpdateWindowTitle(const std::string& sceneName);

    void DrawViewportSettingsPanel();
    void DrawEnvironmentSettingsPanel();
    void DrawAnimationPanel();
    void DrawMaterialsPanel();
    void DrawViewport();
    void DrawToolbar();
    void DrawMenuBar();

    void NewScene();
    void OpenScene();
    void SaveScene(const std::string& path = "");

    void OnScenePlay();
    void OnSceneStop();

    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
    bool OnKeyPressed(KeyPressedEvent& e);

    std::pair<glm::vec3, glm::vec3> CastRay(float x, float y);
    std::pair<float, float> GetMousePosition();

    void Shutdown() { Application::Get().Shutdown(); }
};

} // Editor
} // Amber

