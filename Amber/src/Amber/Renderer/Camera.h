#pragma once

#include <glm/glm.hpp>

namespace Amber
{

class Camera : public RefCounted
{
public:
    Camera() = default;
    Camera(const glm::mat4& projectionMatrix) : m_ProjectionMatrix(projectionMatrix) {}
    virtual ~Camera() = default;

    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

    float& Exposure() { return m_Exposure; }
    float GetExposure() const { return m_Exposure; }

    void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }
    void SetExposure(float exposure) { m_Exposure = exposure; }

protected:
    glm::mat4 m_ProjectionMatrix;
    float m_Exposure = 1.0f;
};

}
