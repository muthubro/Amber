#include "abpch.h"
#include "EditorCamera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Amber/Core/Input.h"

#define PI 3.14159265f

namespace Amber
{

EditorCamera::EditorCamera(const glm::mat4& projectionMatrix)
    : Camera(projectionMatrix)
{
    m_FocalPoint = { 0.0f, 0.0f, 0.0f };
    m_Pitch = -PI / 4.0f;
    m_Yaw = -3.0f * PI / 4.0f;
    m_Distance = 9.0f;

    UpdateCamera();
}

void EditorCamera::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(AB_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
}

void EditorCamera::OnUpdate(Timestep ts)
{
    if (Input::IsKeyPressed(AB_KEY_LEFT_ALT))
    {
        const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
        glm::vec2 delta = (mouse - m_MousePosition) * 0.003f;
        m_MousePosition = mouse;

        if (Input::IsMouseButtonPressed(AB_MOUSE_BUTTON_MIDDLE))
            MousePan(delta);
        else if (Input::IsMouseButtonPressed(AB_MOUSE_BUTTON_LEFT))
            MouseRotate(delta);
        else if (Input::IsMouseButtonPressed(AB_MOUSE_BUTTON_RIGHT))
            MouseZoom(delta.y, false);
    }

    UpdateCamera();
}

void EditorCamera::CalculateOrientation()
{
    m_Orientation = glm::quat(glm::vec3(m_Pitch, m_Yaw, 0.0f));
}

void EditorCamera::CalculatePosition()
{
    m_Position = m_FocalPoint - m_ForwardDirection * m_Distance;
}

void EditorCamera::CalculateViewMatrix()
{
    m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(m_Orientation);
    m_ViewMatrix = glm::inverse(m_ViewMatrix);
    m_ViewProjection = m_ProjectionMatrix * m_ViewMatrix;
}

void EditorCamera::CalculateDirections()
{
    m_UpDirection = glm::rotate(m_Orientation, glm::vec3(0.0f, 1.0f, 0.0f));
    m_RightDirection = glm::rotate(m_Orientation, glm::vec3(1.0f, 0.0f, 0.0f));
    m_ForwardDirection = glm::rotate(m_Orientation, glm::vec3(0.0f, 0.0f, -1.0f));
}

void EditorCamera::UpdateCamera()
{
    CalculateOrientation();
    CalculateDirections();
    CalculatePosition();
    CalculateViewMatrix();
}

bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
{
    float delta = e.GetYOffset();
    MouseZoom(delta);
    return false;
}

void EditorCamera::MousePan(const glm::vec2& delta)
{
    auto [xSpeed, ySpeed] = PanSpeed();
    m_FocalPoint += -m_RightDirection * delta.x * xSpeed * m_Distance;
    m_FocalPoint += m_UpDirection * delta.y * ySpeed * m_Distance;
}

void EditorCamera::MouseRotate(const glm::vec2& delta)
{
    float speed = RotationSpeed();
    float yawSign = m_UpDirection.y < 0.0f ? -1.0f : 1.0f;
    m_Pitch -= delta.y * speed;
    m_Yaw -= yawSign * delta.x * speed;
}

void EditorCamera::MouseZoom(float delta, bool linear)
{
    float speed = ZoomSpeed();
    if (!linear) speed *= speed;
    m_Distance -= delta * speed;
    if (m_Distance < 1.0f)
    {
        m_FocalPoint += m_ForwardDirection;
        m_Distance = 1.0f;
    }
}

std::pair<float, float> EditorCamera::PanSpeed() const
{
    float x = glm::min(m_ViewportWidth / 1000.0f, 2.4f);
    float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = glm::min(m_ViewportHeight / 1000.0f, 2.4f);
    float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    return { xFactor, yFactor };
}

float EditorCamera::RotationSpeed() const
{
    return 1.0f;
}

float EditorCamera::ZoomSpeed() const
{
    float distance = m_Distance * 0.2f;
    distance = glm::max(distance, 0.0f);
    float speed = distance;
    speed = glm::min(speed, 100.0f);
    return speed;
}

}