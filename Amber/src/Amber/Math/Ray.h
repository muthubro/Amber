#pragma once

#include <glm/glm.hpp>

#include "Amber/Math/AABB.h"

namespace Amber
{

struct Ray
{
    glm::vec3 Origin, Direction;

    Ray(const glm::vec3& origin, const glm::vec3& direction)
        : Origin(origin), Direction(direction) {}

    // 't' will hold the distance from ray origin to the intersection point
    bool IntersectsAABB(const AABB& aabb, float& t)
    {
        const glm::vec3& min = aabb.Min, max = aabb.Max;
        
        glm::vec3 invDir = glm::vec3(
            Direction.x ? 1.0 / Direction.x : std::numeric_limits<float>::max(),
            Direction.y ? 1.0 / Direction.y : std::numeric_limits<float>::max(),
            Direction.z ? 1.0 / Direction.z : std::numeric_limits<float>::max());

        // Distances to intersections of ray with the planes of AABB
        // From vector equation of line: r = a + td
        float t_x1 = (min.x - Origin.x) * invDir.x;
        float t_y1 = (min.y - Origin.y) * invDir.y;
        float t_z1 = (min.z - Origin.z) * invDir.z;

        float t_x2 = (max.x - Origin.x) * invDir.x;
        float t_y2 = (max.y - Origin.y) * invDir.y;
        float t_z2 = (max.z - Origin.z) * invDir.z;

        // Say the ray intersects the AABB at point 'p' given by p = a + td
        // t for this point will be the greatest among t_x, t_y, and t_z for the near plane
        // and smallest for the far plane
        float t_min = glm::max(glm::max(glm::min(t_x1, t_x2), glm::min(t_y1, t_y2)), glm::min(t_z1, t_z2));
        float t_max = glm::min(glm::min(glm::max(t_x1, t_x2), glm::max(t_y1, t_y2)), glm::max(t_z1, t_z2));

        // If t_max < 0, AABB lies behind ray
        if (t_min > t_max || t_max < 0.0f)
        {
            t = t_max;
            return false;
        }

        t = t_min;
        return true;
    }

    // Möller–Trumbore Algorithm
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
    bool IntersectsTriangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C, float& t)
    {
        glm::vec3 E1 = B - A;
        glm::vec3 E2 = C - A;

        glm::vec3 N = glm::cross(E1, E2);
        float det = -glm::dot(N, Direction);

        if (det < 1e-6)
        {
            t = 0;
            return false;
        }

        float invDet = 1.0f / det;

        glm::vec3 AO  = Origin - A;
        glm::vec3 DAO = glm::cross(AO, Direction);

        t = glm::dot(AO, N) * invDet;
        float u = glm::dot(E2, DAO) * invDet;
        float v = -glm::dot(E1, DAO) * invDet;

        return (t >= 0.0f && u >= 0.0f && v >= 0.0f && u + v <= 1.0f);
    }
};

}