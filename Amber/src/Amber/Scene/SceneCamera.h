#pragma once

#include "Amber/Renderer/Camera.h"

namespace Amber
{

class SceneCamera : public Camera
{
public:
    enum class ProjectionType
    {
        Perspective = 0, Orthographic = 1
    };

    // Vertical FOV, near clip distance, far clip distance
    struct PerspectiveData
    {
        float FOV = glm::radians(45.0f);
        float Near = 0.1f;
        float Far = 1000.0f;
        float Width = 1280.0f;
        float Height = 720.0f;
    };

    // Size, near clip plane, far clip plane
    struct OrthographicData
    {
        float Size = 10.0f;
        float Near = -1.0f;
        float Far = 1.0f;
        float AspectRatio = 1280.0f / 720.0f;
    };

    SceneCamera();

    const PerspectiveData& GetPerspectiveData() const { return m_PerspectiveData; }
    const OrthographicData& GetOrthographicData() const { return m_OrthographicData; }
    ProjectionType GetProjectionType() const { return m_ProjectionType; }

    void SetPerspective(const PerspectiveData& data);
    void SetPerspectiveData(const PerspectiveData& data) { m_PerspectiveData = data; }
    void SetOrthographic(const OrthographicData& data);
    void SetOrthographicData(const OrthographicData& data) { m_OrthographicData = data; }

    void SetProjectionType(ProjectionType type) { m_ProjectionType = type; }
    void SetViewportSize(uint32_t width, uint32_t height);

private:
    ProjectionType m_ProjectionType = ProjectionType::Perspective;
    PerspectiveData m_PerspectiveData;
    OrthographicData m_OrthographicData;
};

}