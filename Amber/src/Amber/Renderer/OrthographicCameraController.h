#pragma once

#include "Amber/Core/Timestep.h"

#include "Amber/Events/ApplicationEvent.h"
#include "Amber/Events/MouseEvent.h"

#include "Amber/Renderer/OrthographicCamera.h"

namespace Amber
{

    struct CameraBounds
    {
        float Left, Right;
        float Bottom, Top;

        float GetWidth() { return Right - Left; }
        float GetHeight() { return Top - Bottom; }
    };

class OrthographicCameraController
{
public:
    OrthographicCameraController(float aspectRatio, bool enableRotation = false);

    OrthographicCamera& GetCamera() { return m_Camera; }
    const OrthographicCamera& GetCamera() const { return m_Camera; }

    const CameraBounds& GetBounds() const { return m_Bounds; }

    float GetZoomLevel() const { return m_ZoomLevel; }
    void SetZoomLevel(float level) { m_ZoomLevel = level; }

    glm::vec3 GetWorldCoordinates(const glm::vec3& screenCoordinates);

    void SetCameraTranslationSpeed(float translationSpeed) { m_CameraTranslationSpeed = translationSpeed; }
    void SetCameraRotationSpeed(float rotationSpeed) { m_CameraRotationSpeed = rotationSpeed; }

    void OnUpdate(Timestep ts);
    void OnEvent(Event& e);

private:
    float m_AspectRatio;
    float m_ZoomLevel = 1.0f;
    bool m_RotationEnabled;

    CameraBounds m_Bounds;

    OrthographicCamera m_Camera;

    glm::vec3 m_CameraPosition = glm::vec3(0.0f);
    float m_CameraTranslationSpeed = 1.0f;
    
    float m_CameraRotation = 0.0f;
    float m_CameraRotationSpeed = 20.0f;

    bool OnWindowResize(WindowResizeEvent& e);
    bool OnMouseScrolled(MouseScrolledEvent& e);
};

}
