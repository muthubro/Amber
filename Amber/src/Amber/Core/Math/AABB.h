#pragma once

#include <glm/glm.hpp>

namespace Amber
{

struct AABB
{
    glm::vec3 Min, Max;

    AABB()
        : Min(0.0f), Max(0.0f) {}

    AABB(const glm::vec3& min, const glm::vec3& max)
        : Min(min), Max(max) {}

    bool Intersects(const AABB& other) const
    {
        if ((Max.x < other.Min.x) || (Min.x > other.Max.x) ||
            (Max.y < other.Min.y) || (Min.y > other.Max.y) ||
            (Max.z < other.Min.z) || (Min.z > other.Max.z))
                return false;
        return true;
    }
};

}