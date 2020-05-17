#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Amber/Core/Timestep.h"

#include "Amber/Core/Events/Event.h"
#include "Amber/Core/Events/MouseEvent.h"

namespace Amber
{

class Camera
{
public:
    Camera() = default;
    Camera(const glm::mat4& projectionMatrix);

    void OnEvent(Event& e);
    void OnUpdate(Timestep ts);

    const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    const glm::mat4& GetViewProjection() const { return m_ProjectionMatrix * m_ViewMatrix; }

    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::quat& GetOrientation() const { return m_Orientation; }
    
    const glm::vec3& GetUpDirection() const { return m_UpDirection; }
    const glm::vec3& GetRightDirection() const { return m_RightDirection; }
    const glm::vec3& GetForwardDirection() const { return m_ForwardDirection; }

    void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }
    void SetViewportSize(uint32_t width, uint32_t height) { m_ViewportWidth = width; m_ViewportHeight = height; }

    float GetDistance() const { return m_Distance; }

private:
    glm::mat4 m_ViewMatrix, m_ProjectionMatrix;
    glm::vec3 m_Position, m_FocalPoint;
    glm::quat m_Orientation;
    glm::vec3 m_UpDirection, m_RightDirection, m_ForwardDirection;

    float m_Distance;
    float m_Yaw, m_Pitch;

    uint32_t m_ViewportWidth, m_ViewportHeight;

    glm::vec2 m_MousePosition = glm::vec2(0.0f);

    void CalculateOrientation();
    void CalculatePosition();
    void CalculateViewMatrix();
    void SetDirections();

    void UpdateCamera();

    bool OnMouseScroll(MouseScrolledEvent& e);

    void MousePan(const glm::vec2& delta);
    void MouseRotate(const glm::vec2& delta);
    void MouseZoom(float delta);

    std::pair<float, float> PanSpeed() const;
    float RotateSpeed() const;
    float ZoomSpeed() const;
};

}
