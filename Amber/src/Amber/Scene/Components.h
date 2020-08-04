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

    CameraComponent() = default;
    CameraComponent(const SceneCamera& camera, bool primary)
        : Camera(camera), Primary(primary) { }
    CameraComponent(bool primary)
        : Primary(primary) {}

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

struct ScriptComponent
{
    std::string ModuleName;

    operator std::string& () { return ModuleName; }
    operator const std::string& () const { return ModuleName; }
};

struct RigidBody2DComponent
{
    enum class Type
    {
        Static, Dynamic, Kinematic
    };
    Type BodyType = Type::Static;
    float Density = 1.0f;
    float Friction = 1.0f;
    float Restitution = 0.0f;

    void* RuntimeBody = nullptr;

    RigidBody2DComponent() = default;
    RigidBody2DComponent(Type bodyType, float density, float friction)
        : BodyType(bodyType), Density(density), Friction(friction) { }
    RigidBody2DComponent(const RigidBody2DComponent& other)
        : BodyType(other.BodyType), Density(other.Density), Friction(other.Friction), Restitution(other.Restitution) { }
};

struct BoxCollider2DComponent
{
    glm::vec2 Offset = { 0.0f, 0.0f };
    glm::vec2 Size = { 1.0f, 1.0f };

    void* RuntimeFixture = nullptr;

    BoxCollider2DComponent() = default;
    BoxCollider2DComponent(const glm::vec2& offset, const glm::vec2& size)
        : Offset(offset), Size(size) { }
};

struct CircleCollider2DComponent
{
    glm::vec2 Offset = { 0.0f, 0.0f };
    float Radius = 1.0f;

    void* RuntimeFixture = nullptr;

    CircleCollider2DComponent() = default;
    CircleCollider2DComponent(const glm::vec2& offset, float radius)
        : Offset(offset), Radius(radius) { }
};

}
