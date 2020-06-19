#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/Math/AABB.h"

#include "Amber/Renderer/Camera.h"
#include "Amber/Renderer/Mesh.h"

namespace Amber
{

struct TagComponent
{
    std::string Tag;

    operator std::string& () { return Tag; }
    operator const std::string& () const { return Tag; }
};

struct TransformComponent
{
    glm::mat4 Transform = glm::mat4(1.0f);

    operator glm::mat4& () { return Transform; }
    operator const glm::mat4& () const { return Transform; }
};

struct CameraComponent
{
    Amber::Camera Camera;
    bool Primary = true;

    operator Amber::Camera& () { return Camera; }
    operator const Amber::Camera& () const { return Camera; }
};

struct MeshComponent
{
    Ref<Amber::Mesh> Mesh;

    operator Ref<Amber::Mesh>() { return Mesh; }
    operator const Ref<Amber::Mesh>() const { return Mesh; }
};

struct BoxCollider
{
    AABB BoundingBox;

    operator AABB() { return BoundingBox; }
    operator const AABB() const { return BoundingBox; }
};

}
