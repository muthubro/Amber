#include "hzpch.h"
#include "OrthographicCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Hazel
{

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float zNear, float zFar)
	: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, zNear, zFar))
{
	HZ_PROFILE_FUNCTION();

	m_ViewProjectionMatrix = m_ProjectionMatrix* m_ViewMatrix;
}

void OrthographicCamera::SetProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	HZ_PROFILE_FUNCTION();

	m_ProjectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::SetPosition(const glm::vec3& position)
{
	m_Position = position;
	CalculateViewMatrix();
}

void OrthographicCamera::SetRotation(float rotation)
{
	m_Rotation = rotation;
	CalculateViewMatrix();
}

void OrthographicCamera::CalculateViewMatrix()
{
	HZ_PROFILE_FUNCTION();

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, m_Position);
	transform = glm::rotate(transform, glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

	m_ViewMatrix = glm::inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

}