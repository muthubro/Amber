#pragma once

#include <glm/glm.hpp>

#include "Amber/Renderer/Mesh.h"

namespace Amber
{

class MeshFactory
{
public:
    static Ref<Mesh> Plane(float width, float height);
    static Ref<Mesh> Cube(const glm::vec3& center, float length);
    static Ref<Mesh> Sphere(const glm::vec3& center, float radius);
};

}