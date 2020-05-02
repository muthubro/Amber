#include "abpch.h"
#include "OrthographicCameraController.h"

#include "Amber/Core/Application.h"
#include "Amber/Core/Input.h"
#include "Amber/Core/KeyCodes.h"

namespace Amber
{

OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool enableRotation)
	: m_AspectRatio(aspectRatio), m_RotationEnabled(enableRotation),
		m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
		m_Bounds({ -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel }) {}

void OrthographicCameraController::OnUpdate(Timestep ts)
{
	AB_PROFILE_FUNCTION();

	if (Input::IsKeyPressed(AB_KEY_A))
	{
		m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;

	}
	else if (Input::IsKeyPressed(AB_KEY_D))
	{
		m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
	}

	if (Input::IsKeyPressed(AB_KEY_W))
	{
		m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
	}
	else if (Input::IsKeyPressed(AB_KEY_S))
	{
		m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
		m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
	}

	if (m_RotationEnabled)
	{
		if (Input::IsKeyPressed(AB_KEY_Q))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		else if (Input::IsKeyPressed(AB_KEY_E))
			m_CameraRotation -= m_CameraRotationSpeed * ts;

		if (m_CameraRotation > 180.0f)
			m_CameraRotation -= 360.0f;
		else if (m_CameraRotation < -180.0f)
			m_CameraRotation += 360.0f;

		m_Camera.SetRotation(m_CameraRotation);
	}

	m_Camera.SetPosition(m_CameraPosition);
		
	m_CameraTranslationSpeed = m_ZoomLevel;
}

void OrthographicCameraController::OnEvent(Event& e)
{
	AB_PROFILE_FUNCTION();

	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(AB_BIND_EVENT_FN(OrthographicCameraController::OnWindowResize));
	dispatcher.Dispatch<MouseScrolledEvent>(AB_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
}

bool OrthographicCameraController::OnWindowResize(WindowResizeEvent& e)
{
	AB_PROFILE_FUNCTION();

	m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };

	return false;
}

bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
	AB_PROFILE_FUNCTION();

	m_ZoomLevel -= e.GetYOffset() * 0.1f;
	m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
	m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	m_Bounds = { -m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel };

	m_CameraTranslationSpeed = m_ZoomLevel;
	return false;
}

glm::vec3 OrthographicCameraController::GetWorldCoordinates(const glm::vec3& screenCoordinates)
{
	return glm::inverse(m_Camera.GetViewProjectionMatrix()) * glm::vec4(screenCoordinates, 1.0f);
}

}