#pragma once

#include <glm/glm.hpp>

namespace Amber
{
namespace Math
{

struct AABB
{
    glm::vec3 Min, Max;

    AABB();
    AABB(const glm::vec3& min, const glm::vec3& max);

    bool Intersects(const AABB& other) const;
};

} // Math
} // Amber