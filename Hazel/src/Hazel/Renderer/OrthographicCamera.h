#pragma once

#include <glm/glm.hpp>

namespace Hazel 
{

class OrthographicCamera
{
public:
	OrthographicCamera(float left, float right, float bottom, float top, float zNear = -1.0f, float zFar = 1.0f);

	inline const glm::vec3& GetPosition() const { return m_Position; }
	void SetPosition(glm::vec3 position);

	inline const float& GetRotation() const { return m_Rotation; }
	void SetRotation(float rotation);

	inline const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	inline const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	inline const glm::mat4& GetVPMatrix() const { return m_VPMatrix; }

private:
	void CalculateViewMatrix();

	glm::mat4 m_ViewMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_VPMatrix;

	glm::vec3 m_Position;
	float m_Rotation;
};

}
