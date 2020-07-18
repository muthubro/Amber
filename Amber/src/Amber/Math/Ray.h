#pragma once

#include <glm/glm.hpp>

#include "Amber/Math/AABB.h"

namespace Amber
{
namespace Math
{

struct Ray
{
    glm::vec3 Origin, Direction;

    Ray(const glm::vec3& origin, const glm::vec3& direction);

    // 't' will hold the distance from ray origin to the intersection point
    bool IntersectsAABB(const AABB& aabb, float& t);

    // Möller–Trumbore Algorithm
    bool IntersectsTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float& t);
};

} // Math
} // Amber