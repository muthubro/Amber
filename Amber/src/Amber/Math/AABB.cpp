#include "abpch.h"
#include "AABB.h"

namespace Amber
{
namespace Math
{

AABB::AABB()
    : Min(std::numeric_limits<float>::max()), Max(-std::numeric_limits<float>::max())
{
}

AABB::AABB(const glm::vec3& min, const glm::vec3& max)
    : Min(min), Max(max)
{
}

bool AABB::Intersects(const AABB& other) const
{
    if ((Max.x < other.Min.x) || (Min.x > other.Max.x) ||
        (Max.y < other.Min.y) || (Min.y > other.Max.y) ||
        (Max.z < other.Min.z) || (Min.z > other.Max.z))
        return false;
    return true;
}

} // Math
} // Amber