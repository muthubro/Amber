#pragma once

#include <glm/glm.hpp>

namespace Amber
{
namespace Math
{

struct Frustum
{
    glm::vec3 Focus;
    glm::vec3 Front, Up;
    float NearPlane, FarPlane;
    float HorizontalFOV, VerticalFOV;

    Frustum();

    Frustum(const glm::vec3& focus, const glm::vec3& front, const glm::vec3 up,
            float nearPlane, float farPlane,
            float horizontalFOV, float verticalFOV);

    Frustum(const glm::mat4& transform, float verticalFOV, float nearClip, float farClip, float aspectRatio);

    glm::vec3 Right() const;

    std::array<glm::vec3, 8> GetCornerPoints() const;
};

} // Math
} // Amber