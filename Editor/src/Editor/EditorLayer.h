#pragma once

#include <vector>

#include "Amber.h"

namespace Amber
{
namespace Editor
{

class EditorLayer : public Layer
{
public:
    EditorLayer();
    ~EditorLayer();

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

    struct SelectedSubmesh
    {
        Entity Entity;
        Submesh* Mesh;
        float Distance;

        SelectedSubmesh() = default;

        SelectedSubmesh(Amber::Entity entity, Submesh* mesh, float distance)
            : Entity(entity), Mesh(mesh), Distance(distance) {}
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

    Ref<Scene> m_Scene;
    Entity m_CameraEntity;

    glm::vec2 m_ViewportBounds[2];
    bool m_AllowViewportCameraEvents = true;
    bool m_EnableOverlay = true;
    bool m_ShowGrid = true;
    float m_GridResolution = 0.025f;
    float m_GridScale = 16.025f;
    float m_GridSize = 16.025f;

    std::vector<SelectedSubmesh> m_SelectionContext;
    float m_SnapValue = 0.5f;
    int m_GizmoMode = -1;
    SelectionMode m_SelectionMode = SelectionMode::Entity;

    bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
    bool OnKeyPressed(KeyPressedEvent& e);

    std::pair<glm::vec3, glm::vec3> CastRay(float x, float y);
    std::pair<float, float> GetMousePosition();

    void Shutdown() { Application::Get().Shutdown(); }
};

} // Editor
} // Amber
