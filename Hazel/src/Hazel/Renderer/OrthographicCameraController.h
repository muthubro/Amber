#pragma once

#include "Hazel/Core/Timestep.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include "Hazel/Renderer/OrthographicCamera.h"

namespace Hazel
{

class OrthographicCameraController
{
public:
	OrthographicCameraController(float aspectRatio, bool enableRotation = false);

	inline OrthographicCamera& GetCamera() { return m_Camera; }
	inline const OrthographicCamera& GetCamera() const { return m_Camera; }

	inline void SetCameraTranslationSpeed(float translationSpeed) { m_CameraTranslationSpeed = translationSpeed; }
	inline void SetCameraRotationSpeed(float rotationSpeed) { m_CameraRotationSpeed = rotationSpeed; }

	void OnUpdate(Timestep ts);
	void OnEvent(Event& e);

private:
	bool OnWindowResize(WindowResizeEvent& e);
	bool OnMouseScrolled(MouseScrolledEvent& e);

	float m_AspectRatio;
	float m_ZoomLevel = 1.0f;
	bool m_RotationEnabled;

	OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition = glm::vec3(0.0f);
	float m_CameraTranslationSpeed = 1.0f;
	
	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 20.0f;
};

}
