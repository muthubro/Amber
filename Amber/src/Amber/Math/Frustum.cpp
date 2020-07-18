#include "abpch.h"
#include "Frustum.h"

namespace Amber
{
namespace Math
{

Frustum::Frustum()
{
}

Frustum::Frustum(const glm::vec3& focus, const glm::vec3& front, const glm::vec3 up, 
                 float nearPlane, float farPlane, float horizontalFOV, float verticalFOV)
    : Focus(focus), Front(front), Up(up), NearPlane(nearPlane), FarPlane(farPlane),
    HorizontalFOV(horizontalFOV), VerticalFOV(verticalFOV)
{
}

Frustum::Frustum(const glm::mat4& transform, float verticalFOV, float nearClip, float farClip, float aspectRatio)
{
    Front = glm::normalize(glm::mat3(transform) * glm::vec3(0.0f, 0.0f, -1.0f));
    Up = glm::normalize(glm::mat3(transform) * glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 position(transform[3]);
    Focus = position;

    NearPlane = nearClip;
    FarPlane = farClip;

    VerticalFOV = verticalFOV;
    HorizontalFOV = glm::atan(aspectRatio * glm::tan(verticalFOV * 0.5f)) * 2.0f;
}

glm::vec3 Frustum::Right() const
{
    return glm::cross(Front, Up);
}

std::array<glm::vec3, 8> Frustum::GetCornerPoints() const
{
    float tanHorizontal = glm::tan(HorizontalFOV * 0.5f);
    float tanVertical = glm::tan(VerticalFOV * 0.5f);

    glm::vec3 right = Right();

    glm::vec3 nearHalfWidth = tanHorizontal * NearPlane * right;
    glm::vec3 nearHalfHeight = tanVertical * NearPlane * Up;
    glm::vec3 farHalfWidth = tanHorizontal * FarPlane * right;
    glm::vec3 farHalfHeight = tanVertical * FarPlane * Up;

    glm::vec3 nearCenter = Focus + NearPlane * Front;
    glm::vec3 farCenter = Focus + FarPlane * Front;

    return std::array<glm::vec3, 8>{
        nearCenter - nearHalfWidth - nearHalfHeight,
        nearCenter + nearHalfWidth - nearHalfHeight,
        nearCenter + nearHalfWidth + nearHalfHeight,
        nearCenter - nearHalfWidth + nearHalfHeight,
        farCenter - farHalfWidth - farHalfHeight,
        farCenter + farHalfWidth - farHalfHeight,
        farCenter + farHalfWidth + farHalfHeight,
        farCenter - farHalfWidth + farHalfHeight,
    };
}

} // Math
} // Amber