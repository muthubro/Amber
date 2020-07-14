#include "abpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Amber
{

SceneCamera::SceneCamera()
{
    SetPerspective(m_PerspectiveData);
}

void SceneCamera::Update()
{
    switch (m_ProjectionType)
    {
        case ProjectionType::Perspective:
        {
            m_ProjectionMatrix = glm::perspectiveFov(
                m_PerspectiveData.FOV, m_PerspectiveData.Width, m_PerspectiveData.Height,
                m_PerspectiveData.Near, m_PerspectiveData.Far);
            break;
        }

        case ProjectionType::Orthographic:
        {
            float width = m_OrthographicData.AspectRatio * m_OrthographicData.Size;
            float height = m_OrthographicData.Size;
            m_ProjectionMatrix = glm::ortho(
                -width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f,
                m_OrthographicData.Near, m_OrthographicData.Far);
            break;
        }
    }
}

void SceneCamera::SetPerspective(const PerspectiveData& data)
{
    m_PerspectiveData = data;
    m_ProjectionType = ProjectionType::Perspective;
}

void SceneCamera::SetOrthographic(const OrthographicData  & data)
{
    m_OrthographicData = data;
    m_ProjectionType = ProjectionType::Orthographic;
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
{
    m_PerspectiveData.Width = (float)width;
    m_PerspectiveData.Height = (float)height;
    m_OrthographicData.AspectRatio = (float)width / height;
}

}