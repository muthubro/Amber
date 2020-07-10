#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/UUID.h"

#include "Amber/Math/AABB.h"

#include "Amber/Renderer/Camera.h"
#include "Amber/Renderer/Mesh.h"

#include "Amber/Scene/SceneCamera.h"

namespace Amber
{

struct IDComponent
{
    UUID ID = 0;

    operator UUID& () { return ID; }
    operator const UUID& () const { return ID; }
};

struct SceneComponent
{
    UUID SceneID;

    operator UUID& () { return SceneID; }
    operator const UUID& () const { return SceneID; }
};

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
    SceneCamera Camera;
    bool Primary = true;

    operator SceneCamera& () { return Camera; }
    operator const SceneCamera& () const { return Camera; }
};

struct MeshComponent
{
    Ref<Amber::Mesh> Mesh;

    operator Ref<Amber::Mesh>() { return Mesh; }
    operator const Ref<Amber::Mesh>() const { return Mesh; }

    operator bool() { return Mesh; }
    operator bool() const { return Mesh; }
};

struct BoxColliderComponent
{
    Math::AABB BoundingBox;

    operator Math::AABB&() { return BoundingBox; }
    operator const Math::AABB&() const { return BoundingBox; }
};

struct ScriptComponent
{
    std::string ModuleName;

    operator std::string& () { return ModuleName; }
    operator const std::string& () const { return ModuleName; }
};

}
