#include "abpch.h"
#include "SceneSerializer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <yaml-cpp/yaml.h>

#include "Amber/Math/Transforms.h"

#include "Amber/Renderer/MeshFactory.h"
#include "Amber/Renderer/Renderer.h"

#include "Amber/Scene/Components.h"
#include "Amber/Scene/Entity.h"

#include "Amber/Script/ScriptEngine.h"

namespace Amber
{

struct Script
{
    std::string ModuleName;
    Amber::ScriptModuleFieldMap FieldMap;
};

} // Amber

namespace YAML
{

template<>
struct convert<glm::vec2>
{
    static Node encode(const glm::vec2& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        return node;
    }

    static bool decode(const Node& node, glm::vec2& rhs)
    {
        if (!node.IsSequence() || node.size() != 2)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        return true;
    }
};

Emitter& operator<<(Emitter& out, const glm::vec2& v)
{
    out << Flow;
    out << BeginSeq << v.x << v.y << EndSeq;
    return out;
}

template<>
struct convert<glm::vec3>
{
    static Node encode(const glm::vec3& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node& node, glm::vec3& rhs)
    {
        if (!node.IsSequence() || node.size() != 3)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

Emitter& operator<<(Emitter& out, const glm::vec3& v)
{
    out << Flow;
    out << BeginSeq << v.x << v.y << v.z << EndSeq;
    return out;
}

template<>
struct convert<glm::vec4>
{
    static Node encode(const glm::vec4& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        return node;
    }

    static bool decode(const Node& node, glm::vec4& rhs)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};

Emitter& operator<<(Emitter& out, const glm::vec4& v)
{
    out << Flow;
    out << BeginSeq << v.x << v.y << v.z << v.w << EndSeq;
    return out;
}

template<>
struct convert<glm::quat>
{
    static Node encode(const glm::quat& rhs)
    {
        Node node;
        node.push_back(rhs.w);
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        return node;
    }

    static bool decode(const Node& node, glm::quat& rhs)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        rhs.w = node[0].as<float>();
        rhs.x = node[1].as<float>();
        rhs.y = node[2].as<float>();
        rhs.z = node[3].as<float>();
        return true;
    }
};

Emitter& operator<<(Emitter& out, const glm::quat& v)
{
    out << Flow;
    out << BeginSeq << v.w << v.x << v.y << v.z << EndSeq;
    return out;
}

template<>
struct convert<glm::mat4>
{
    static Node encode(const glm::mat4& rhs)
    {
        Node node;
        node.push_back(rhs[0]);
        node.push_back(rhs[1]);
        node.push_back(rhs[2]);
        node.push_back(rhs[3]);
        return node;
    }

    static bool decode(const Node& node, glm::mat4& rhs)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        for (uint8_t i = 0; i < 4; i++)
        {
            if (node[i].size() != 4)
                return false;

            rhs[i] = node[i].as<glm::vec4>();
        }
        return true;
    }
};

Emitter& operator<<(Emitter& out, const glm::mat4& m)
{
    out << Flow;
    out << BeginSeq << m[0] << m[1] << m[2] << m[3] << EndSeq;
    return out;
}

using namespace Amber;

template<>
struct convert<Light>
{
    static Node encode(const Light& rhs)
    {
        Node node;
        node["Direction"] = rhs.Direction;
        node["Radiance"] = rhs.Radiance;
        node["Multiplier"] = rhs.Multiplier;
        return node;
    }

    static bool decode(const Node& node, Light& rhs)
    {
        if (!node.IsMap() || !node["Direction"] || !node["Radiance"] || !node["Multiplier"])
            return false;

        rhs.Direction = node["Direction"].as<glm::vec3>();
        rhs.Radiance = node["Radiance"].as<glm::vec3>();
        rhs.Multiplier = node["Multiplier"].as<float>();
        return true;
    }
};

Emitter& operator<<(Emitter& out, const Light& light)
{
    out << BeginMap;

    out << Key << "Direction";
    out << Value << light.Direction;

    out << Key << "Radiance";
    out << Value << light.Radiance;

    out << Key << "Multiplier";
    out << Value << light.Multiplier;

    out << EndMap;
    return out;
}

template<>
struct convert<TagComponent>
{
    static Node encode(const TagComponent& rhs)
    {
        Node node;
        node["Tag"] = rhs.Tag;
        return node;
    }

    static bool decode(const Node& node, TagComponent& rhs)
    {
        if (!node.IsMap() || !node["Tag"])
            return false;

        rhs.Tag = node["Tag"].as<std::string>();
        return true;
    }
};

Emitter& operator<<(Emitter& out, const TagComponent& tagComponent)
{
    out << BeginMap;

    out << Key << "Tag";
    out << Value << tagComponent.Tag;

    out << EndMap;
    return out;
}

template<>
struct convert<TransformComponent>
{
    static Node encode(const TransformComponent& rhs)
    {
        auto& [position, rotation, scale] = Math::DecomposeTransform(rhs.Transform);

        Node node;
        node["Position"] = position;
        node["Rotation"] = rotation;
        node["Scale"] = scale;
        return node;
    }

    static bool decode(const Node& node, TransformComponent& rhs)
    {
        if (!node.IsMap() || !node["Position"] || !node["Rotation"] || !node["Scale"])
            return false;

        rhs.Transform =
            glm::translate(glm::mat4(1.0f), node["Position"].as<glm::vec3>()) *
            glm::toMat4(node["Rotation"].as<glm::quat>()) *
            glm::scale(glm::mat4(1.0f), node["Scale"].as<glm::vec3>());
        return true;
    }
};

Emitter& operator<<(Emitter& out, const TransformComponent& transformComponent)
{
    auto& [position, rotation, scale] = Math::DecomposeTransform(transformComponent.Transform);

    out << BeginMap;

    out << Key << "Position";
    out << Value << position;

    out << Key << "Rotation";
    out << Value << rotation;

    out << Key << "Scale";
    out << Value << scale;

    out << EndMap;
    return out;
}

template<>
struct convert<CameraComponent>
{
    static Node encode(const CameraComponent& rhs)
    {
        auto& camera = rhs.Camera;
        auto& perspective = camera.GetPerspectiveData();
        auto& orthographic = camera.GetOrthographicData();

        Node node;
        node["Primary"] = rhs.Primary;

        node["Projection"] = camera.GetProjectionMatrix();
        node["Exposure"] = camera.GetExposure();

        node["Type"] = (uint32_t)camera.GetProjectionType();

        node["Perspective"]["FOV"]      = perspective.FOV;
        node["Perspective"]["Near"]     = perspective.Near;
        node["Perspective"]["Far"]      = perspective.Far;
        node["Perspective"]["Width"]    = perspective.Width;
        node["Perspective"]["Height"]   = perspective.Height;
        
        node["Orthographic"]["Size"]        = orthographic.Size;
        node["Orthographic"]["Near"]        = orthographic.Near;
        node["Orthographic"]["Far"]         = orthographic.Far;
        node["Orthographic"]["AspectRatio"] = orthographic.AspectRatio;

        return node;
    }

    static bool decode(const Node& node, CameraComponent& rhs)
    {
        if (!node.IsMap())
            return false;

        if (!node["Primary"])
            rhs.Primary = false;
        else
            rhs.Primary = node["Primary"].as<bool>();

        auto& camera = rhs.Camera;

        if (!node["Exposure"] || !node["Type"])
            return false;

        camera.SetExposure(node["Exposure"].as<float>());
        auto type = (SceneCamera::ProjectionType)node["Type"].as<uint32_t>();

        if (!node["Perspective"] || !node["Perspective"].IsMap())
            return false;

        if (!node["Perspective"]["FOV"] ||
            !node["Perspective"]["Near"] ||
            !node["Perspective"]["Far"] ||
            !node["Perspective"]["Width"] ||
            !node["Perspective"]["Height"])
            return false;

        SceneCamera::PerspectiveData perspective;
        perspective.FOV = node["Perspective"]["FOV"].as<float>();
        perspective.Near = node["Perspective"]["Near"].as<float>();
        perspective.Far = node["Perspective"]["Far"].as<float>();
        perspective.Width = node["Perspective"]["Width"].as<float>();
        perspective.Height = node["Perspective"]["Height"].as<float>();
        camera.SetPerspectiveData(perspective);

        if (!node["Orthographic"]["Size"] ||
            !node["Orthographic"]["Near"] ||
            !node["Orthographic"]["Far"] ||
            !node["Orthographic"]["AspectRatio"])
            return false;

        SceneCamera::OrthographicData orthographic;
        orthographic.Size = node["Orthographic"]["Size"].as<float>();
        orthographic.Near = node["Orthographic"]["Near"].as<float>();
        orthographic.Far = node["Orthographic"]["Far"].as<float>();
        orthographic.AspectRatio = node["Orthographic"]["AspectRatio"].as<float>();
        camera.SetOrthographicData(orthographic);

        camera.SetProjectionType(type);
        return true;
    }
};

Emitter& operator<<(Emitter& out, const CameraComponent& cameraComponent)
{
    auto& camera = cameraComponent.Camera;
    auto& perspective = camera.GetPerspectiveData();
    auto& orthographic = camera.GetOrthographicData();

    out << BeginMap;

    out << Key << "Primary";
    out << Value << cameraComponent.Primary;

    out << Key << "Exposure";
    out << Value << camera.GetExposure();

    out << Key << "Type";
    out << Value << (uint32_t)camera.GetProjectionType();

    out << Key << "Perspective";
    out << Value << BeginMap;

    out << Key << "FOV";
    out << Value << perspective.FOV;
    out << Key << "Near";
    out << Value << perspective.Near;
    out << Key << "Far";
    out << Value << perspective.Far;
    out << Key << "Width";
    out << Value << perspective.Width;
    out << Key << "Height";
    out << Value << perspective.Height;

    out << EndMap;  // Perspective

    out << Key << "Orthographic";
    out << Value << BeginMap;

    out << Key << "Size";
    out << Value << orthographic.Size;
    out << Key << "Near";
    out << Value << orthographic.Near;
    out << Key << "Far";
    out << Value << orthographic.Far;
    out << Key << "AspectRatio";
    out << Value << orthographic.AspectRatio;

    out << EndMap;  // Orthographic

    out << EndMap;  // CameraComponent
    return out;
}

template<>
struct convert<MeshComponent>
{
    static Node encode(const MeshComponent& rhs)
    {
        Node node;
        node["AssetPath"] = rhs.Mesh->GetAssetPath();

        auto shader = rhs.Mesh->GetMaterial()->GetShader();

        node["ShaderType"] = (uint32_t)shader->GetType();
        node["ShaderName"] = shader->GetName();

        return node;
    }

    static bool decode(const Node& node, MeshComponent& rhs)
    {
        if (!node.IsMap() || !node["AssetPath"])
            return false;

        // TODO: Find a better way to do this
        std::string filepath = node["AssetPath"].as<std::string>();
        if (filepath == "Cube")
            rhs.Mesh = MeshFactory::Cube(glm::vec3(0.0f), 1.0f);
        else if (filepath == "Sphere")
            rhs.Mesh = MeshFactory::Sphere(glm::vec3(0.0f), 1.0f);
        else if (filepath == "Plane")
            rhs.Mesh = MeshFactory::Plane(1.0f, 1.0f);
        else
            rhs.Mesh = Ref<Mesh>::Create(filepath);

        auto material = rhs.Mesh->GetMaterial();
        auto shaderType = (ShaderType)node["ShaderType"].as<uint32_t>();
        std::string shaderName = node["ShaderName"].as<std::string>();

        if (shaderType != material->GetShader()->GetType() || shaderName != material->GetShader()->GetName())
        {
            if (shaderType == ShaderType::None)
                material->Reset(Renderer::GetShaderLibrary()->Get(shaderName));
            else
                material->Reset(Renderer::GetShaderLibrary()->Get(shaderType));
        }

        return true;
    }
};

Emitter& operator<<(Emitter& out, const MeshComponent& meshComponent)
{
    out << BeginMap;

    out << Key << "AssetPath";
    out << Value << meshComponent.Mesh->GetAssetPath();

    auto shader = meshComponent.Mesh->GetMaterial()->GetShader();

    out << Key << "ShaderType";
    out << Value << (uint32_t)shader->GetType();

    out << Key << "ShaderName";
    out << Value << shader->GetName();

    out << EndMap;
    return out;
}

template<>
struct convert<SpriteRendererComponent>
{
    static Node encode(const SpriteRendererComponent& rhs)
    {
        Node node;
        node["Color"] = rhs.Color;
        node["Texture"] = rhs.Texture ? rhs.Texture->GetAssetPath() : "";
        for (uint32_t i = 0; i < 4; i++)
            node["TexCoords"][i] = rhs.TexCoords[i];
        node["TilingFactor"] = rhs.TilingFactor;

        return node;
    }

    static bool decode(const Node& node, SpriteRendererComponent& rhs)
    {
        if (!node.IsMap() || !node["Color"] || !node["Texture"] || !node["TexCoords"] || !node["TilingFactor"])
            return false;

        rhs.Color = node["Color"].as<glm::mat4>();

        std::string texturePath = node["Texture"].as<std::string>();
        rhs.Texture = texturePath.empty() ? nullptr : Texture2D::Create(texturePath, false, true, TextureWrap::Repeat, TextureFilter::Nearest);

        for (uint32_t i = 0; i < 4; i++)
            rhs.TexCoords[i] = node["TexCoords"][i].as<glm::vec2>();
        rhs.TilingFactor = node["TilingFactor"].as<float>();

        return true;
    }
};

Emitter& operator<<(Emitter& out, const SpriteRendererComponent& spriteRendererComponent)
{
    out << BeginMap;

    out << Key << "Color";
    out << Value << spriteRendererComponent.Color;

    out << Key << "Texture";
    out << Value << (spriteRendererComponent.Texture ? spriteRendererComponent.Texture->GetAssetPath() : "");
    
    out << Key << "TexCoords";
    out << Value << BeginSeq;
    
    for (uint32_t i = 0; i < 4; i++)
        out << spriteRendererComponent.TexCoords[i];
    
    out << EndSeq;
    
    out << Key << "TilingFactor";
    out << Value << spriteRendererComponent.TilingFactor;

    out << EndMap;
    return out;
}

static void EncodeFields(const ScriptModuleFieldMap& moduleFieldMap, const std::string& moduleName, Node& fields)
{
    const auto& fieldMap = moduleFieldMap.at(moduleName);
    uint32_t index = 0;
    for (auto& [fieldName, field] : fieldMap)
    {
        fields[index]["Name"] = fieldName;
        fields[index]["Type"] = (uint32_t)field.Type;

        switch (field.Type)
        {
            case FieldType::None:
            {
                Node subFields;
                EncodeFields(moduleFieldMap, fieldName, subFields);
                fields[index]["Value"] = subFields;
                break;
            }

            case FieldType::Bool:
                fields[index]["Value"] = field.GetStoredValue<bool>();
                break;

            case FieldType::Int:
                fields[index]["Value"] = field.GetStoredValue<int32_t>();
                break;

            case FieldType::UInt:
                fields[index]["Value"] = field.GetStoredValue<uint32_t>();
                break;

            case FieldType::Float:
                fields[index]["Value"] = field.GetStoredValue<float>();
                break;

            case FieldType::Vec2:
                fields[index]["Value"] = field.GetStoredValue<glm::vec2>();
                break;

            case FieldType::Vec3:
                fields[index]["Value"] = field.GetStoredValue<glm::vec3>();
                break;

            case FieldType::Vec4:
                fields[index]["Value"] = field.GetStoredValue<glm::vec4>();
                break;

            case FieldType::Color:
                fields[index]["Value"] = field.GetStoredValue<glm::vec4>();
                break;
        }
        index++;
    }
}

static bool DecodeFields(const Node& node, ScriptModuleFieldMap& moduleFieldMap, const std::string& moduleName)
{
    auto& fieldMap = moduleFieldMap[moduleName];
    for (auto& field : node)
    {
        if (!field.IsMap() || !field["Name"] || !field["Type"] || !field["Value"])
            return false;

        std::string fieldName = field["Name"].as<std::string>();
        FieldType fieldType = (FieldType)field["Type"].as<uint32_t>();

        if (fieldMap.find(fieldName) == fieldMap.end())
        {
            PublicField publicField(fieldName, fieldType);
            fieldMap.emplace(fieldName, std::move(publicField));
        }

        switch (fieldType)
        {
            case FieldType::None:
            {
                auto& subFields = field["Value"];
                return DecodeFields(subFields, moduleFieldMap, fieldName);
            }

            case FieldType::Bool:
                fieldMap.at(fieldName).SetStoredValue<bool>(field["Value"].as<bool>());
                break;

            case FieldType::Int:
                fieldMap.at(fieldName).SetStoredValue<int32_t>(field["Value"].as<int32_t>());
                break;

            case FieldType::UInt:
                fieldMap.at(fieldName).SetStoredValue<uint32_t>(field["Value"].as<uint32_t>());
                break;

            case FieldType::Float:
                fieldMap.at(fieldName).SetStoredValue<float>(field["Value"].as<float>());
                break;

            case FieldType::Vec2:
                fieldMap.at(fieldName).SetStoredValue<glm::vec2>(field["Value"].as<glm::vec2>());
                break;

            case FieldType::Vec3:
                fieldMap.at(fieldName).SetStoredValue<glm::vec3>(field["Value"].as<glm::vec3>());
                break;

            case FieldType::Vec4:
                fieldMap.at(fieldName).SetStoredValue<glm::vec4>(field["Value"].as<glm::vec4>());
                break;

            case FieldType::Color:
                fieldMap.at(fieldName).SetStoredValue<glm::vec4>(field["Value"].as<glm::vec4>());
                break;
        }
    }

    return true;
}

template<>
struct convert<Script>
{
    static Node encode(const Script& rhs)
    {
        Node node;
        node["ModuleName"] = rhs.ModuleName;

        if (!rhs.FieldMap.empty())
        {
            Node fields;
            EncodeFields(rhs.FieldMap, rhs.ModuleName, fields);
            node["Fields"] = fields;
        }

        return node;
    }

    static bool decode(const Node& node, Script& rhs)
    {
        if (!node.IsMap() || !node["ModuleName"])
            return false;

        rhs.ModuleName = node["ModuleName"].as<std::string>();

        auto& fieldMap = rhs.FieldMap;
        if (node["Fields"])
        {
            const auto& fields = node["Fields"];
            return DecodeFields(fields, fieldMap, rhs.ModuleName);
        }
        return true;
    }
};

Emitter& operator<<(Emitter& out, const Script& script)
{
    out << BeginMap;

    out << Key << "ModuleName";
    out << Value << script.ModuleName;

    Node fields;
    EncodeFields(script.FieldMap, script.ModuleName, fields);
    if (fields.size() != 0)
    {
        out << Key << "Fields";
        out << Value << fields;
    }

    out << EndMap; // Script
    return out;
}

template<>
struct convert<RigidBody2DComponent>
{
    static Node encode(const RigidBody2DComponent& rhs)
    {
        Node node;
        node["BodyType"] = (uint32_t)rhs.BodyType;
        node["Density"] = rhs.Density;
        node["Friction"] = rhs.Friction;
        node["Restitution"] = rhs.Restitution;

        return node;
    }

    static bool decode(const Node& node, RigidBody2DComponent& rhs)
    {
        if (!node.IsMap() || !node["BodyType"] || !node["Density"] || !node["Friction"] || !node["Restitution"])
            return false;

        rhs.BodyType = (RigidBody2DComponent::Type)node["BodyType"].as<uint32_t>();
        rhs.Density = node["Density"].as<float>();
        rhs.Friction = node["Friction"].as<float>();
        rhs.Restitution = node["Restitution"].as<float>();
        return true;
    }
};

Emitter& operator<<(Emitter& out, const RigidBody2DComponent& rigidBody2DComponent)
{
    out << BeginMap;

    out << Key << "BodyType";
    out << Value << (uint32_t)rigidBody2DComponent.BodyType;

    out << Key << "Density";
    out << Value << rigidBody2DComponent.Density;

    out << Key << "Friction";
    out << Value << rigidBody2DComponent.Friction;

    out << Key << "Restitution";
    out << Value << rigidBody2DComponent.Restitution;

    out << EndMap;
    return out;
}

template<>
struct convert<BoxCollider2DComponent>
{
    static Node encode(const BoxCollider2DComponent& rhs)
    {
        Node node;
        node["Offset"] = rhs.Offset;
        node["Size"] = rhs.Size;

        return node;
    }

    static bool decode(const Node& node, BoxCollider2DComponent& rhs)
    {
        if (!node.IsMap() || !node["Offset"] || !node["Size"])
            return false;

        rhs.Offset = node["Offset"].as<glm::vec2>();
        rhs.Size = node["Size"].as<glm::vec2>();
        return true;
    }
};

Emitter& operator<<(Emitter& out, const BoxCollider2DComponent& boxCollider2DComponent)
{
    out << BeginMap;

    out << Key << "Offset";
    out << Value << boxCollider2DComponent.Offset;

    out << Key << "Size";
    out << Value << boxCollider2DComponent.Size;

    out << EndMap;
    return out;
}

template<>
struct convert<CircleCollider2DComponent>
{
    static Node encode(const CircleCollider2DComponent& rhs)
    {
        Node node;
        node["Offset"] = rhs.Offset;
        node["Radius"] = rhs.Radius;

        return node;
    }

    static bool decode(const Node& node, CircleCollider2DComponent& rhs)
    {
        if (!node.IsMap() || !node["Offset"] || !node["Radius"])
            return false;

        rhs.Offset = node["Offset"].as<glm::vec2>();
        rhs.Radius = node["Radius"].as<float>();
        return true;
    }
};

Emitter& operator<<(Emitter& out, const CircleCollider2DComponent& circleCollider2DComponent)
{
    out << BeginMap;

    out << Key << "Offset";
    out << Value << circleCollider2DComponent.Offset;

    out << Key << "Radius";
    out << Value << circleCollider2DComponent.Radius;

    out << EndMap;
    return out;
}

} // YAML

namespace Amber
{

SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
    : m_Scene(scene)
{
}

void SceneSerializer::SerializeEntity(YAML::Emitter& out, const Entity& entity)
{
    UUID entityID = entity.GetUUID();
    UUID sceneID = m_Scene->GetUUID();

    out << YAML::BeginMap;

    out << YAML::Key << "Entity";
    out << YAML::Value << entityID;

    if (entity.HasComponent<TagComponent>())
    {
        out << YAML::Key << "TagComponent";
        out << YAML::Value << entity.GetComponent<TagComponent>();
    }

    if (entity.HasComponent<TransformComponent>())
    {
        out << YAML::Key << "TransformComponent";
        out << YAML::Value << entity.GetComponent<TransformComponent>();
    }

    if (entity.HasComponent<CameraComponent>())
    {
        out << YAML::Key << "CameraComponent";
        out << YAML::Value << entity.GetComponent<CameraComponent>();
    }

    if (entity.HasComponent<MeshComponent>())
    {
        out << YAML::Key << "MeshComponent";
        out << YAML::Value << entity.GetComponent<MeshComponent>();
    }

    if (entity.HasComponent<SpriteRendererComponent>())
    {
        out << YAML::Key << "SpriteRendererComponent";
        out << YAML::Value << entity.GetComponent<SpriteRendererComponent>();
    }

    if (entity.HasComponent<ScriptComponent>())
    {
        std::string moduleName = entity.GetComponent<ScriptComponent>();
        const auto& fieldMap = ScriptEngine::GetEntityInstanceData(sceneID, entityID).ModuleFieldMap;
        Script script{ moduleName, fieldMap };

        out << YAML::Key << "ScriptComponent";
        out << YAML::Value << script;
    }

    if (entity.HasComponent<RigidBody2DComponent>())
    {
        out << YAML::Key << "RigidBody2DComponent";
        out << YAML::Value << entity.GetComponent<RigidBody2DComponent>();
    }

    if (entity.HasComponent<BoxCollider2DComponent>())
    {
        out << YAML::Key << "BoxCollider2DComponent";
        out << YAML::Value << entity.GetComponent<BoxCollider2DComponent>();
    }

    if (entity.HasComponent<CircleCollider2DComponent>())
    {
        out << YAML::Key << "CircleCollider2DComponent";
        out << YAML::Value << entity.GetComponent<CircleCollider2DComponent>();
    }

    out << YAML::EndMap; // Entity
}

void SceneSerializer::SerializeEnvironment(YAML::Emitter& out)
{
    out << YAML::BeginMap;

    const auto& environment = m_Scene->GetEnvironment();
    out << YAML::Key << "AssetPath";
    out << YAML::Value << environment.Filepath;

    out << YAML::Key << "Rotation";
    out << YAML::Value << environment.Rotation;

    out << YAML::Key << "Light";
    out << YAML::Value << m_Scene->GetLight();

    out << YAML::EndMap; // Environment
}

void SceneSerializer::Serialize(const std::string& filepath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;

    out << YAML::Key << "Scene";
    out << YAML::Value << m_Scene->GetDebugName();

    out << YAML::Key << "Environment";
    out << YAML::Value;
    SerializeEnvironment(out);

    out << YAML::Key << "Gravity";
    out << YAML::Value << m_Scene->GetPhysics2DGravity();

    out << YAML::Key << "Entities";
    out << YAML::Value << YAML::BeginSeq;

    auto& entities = m_Scene->GetAllEntities();
    for (auto& entity : entities)
        SerializeEntity(out, { entity, m_Scene.Raw() });

    out << YAML::EndSeq; // Entities
    out << YAML::EndMap;

    std::ofstream file(filepath);
    file << out.c_str();
}

void SceneSerializer::SerializeRuntime(const std::string& filepath)
{
    AB_CORE_ASSERT(false, "Not implemented yet!");
}

static void SetFields(ScriptModuleFieldMap& dstModuleFieldMap, ScriptModuleFieldMap& srcModuleFieldMap, const std::string& moduleName)
{
    auto& dstFieldMap = dstModuleFieldMap[moduleName];
    auto& srcFieldMap = srcModuleFieldMap[moduleName];
    for (auto& [fieldName, field] : srcFieldMap)
    {
        if (field.Type == FieldType::None)
        {
            SetFields(dstModuleFieldMap, srcModuleFieldMap, fieldName);
        }
        else
        {
            if (dstFieldMap.find(fieldName) != dstFieldMap.end())
                dstFieldMap[fieldName] = std::move(field);
        }
    }
}

bool SceneSerializer::Deserialize(const std::string& filepath)
{
    UUID sceneID = m_Scene->GetUUID();
    std::ifstream file(filepath);
    YAML::Node data = YAML::Load(file);

    if (!data["Scene"])
        return false;

    std::string sceneName = data["Scene"].as<std::string>();
    AB_CORE_INFO("Deserializing scene '{0}'", sceneName);

    m_Scene->SetDebugName(sceneName);

    auto environment = data["Environment"];
    if (environment)
    {
        std::string envPath = environment["AssetPath"].as<std::string>();
        m_Scene->SetEnvironment(Environment::Load(envPath));

        Light light = environment["Light"].as<Light>();
        m_Scene->SetLight(light);
    }

    auto entities = data["Entities"];
    if (entities)
    {
        for (auto entity : entities)
        {
            uint64_t entityID = entity["Entity"].as<uint64_t>();

            if (!entity["TagComponent"] || !entity["TransformComponent"])
                return false;

            auto tag = entity["TagComponent"].as<TagComponent>();
            Entity deserializedEntity = m_Scene->CreateEntity(entityID, tag.Tag);
            AB_CORE_INFO("Deserializing entity with ID = {0}, name = {1}", entityID, tag.Tag);

            auto transform = entity["TransformComponent"].as<TransformComponent>();
            deserializedEntity.AddComponent<TransformComponent>(transform.Transform);

            auto cameraNode = entity["CameraComponent"];
            if (cameraNode)
            {
                auto camera = cameraNode.as<CameraComponent>();
                deserializedEntity.AddComponent<CameraComponent>(camera.Camera, camera.Primary);
            }

            auto meshNode = entity["MeshComponent"];
            if (meshNode)
            {
                auto mesh = meshNode.as<MeshComponent>();
                deserializedEntity.AddComponent<MeshComponent>(mesh.Mesh);
            }

            auto spriteRendererNode = entity["SpriteRendererComponent"];
            if (spriteRendererNode)
            {
                auto spriteRenderer = spriteRendererNode.as<SpriteRendererComponent>();
                deserializedEntity.AddComponent<SpriteRendererComponent>(spriteRenderer);
            }

            auto scriptNode = entity["ScriptComponent"];
            if (scriptNode)
            {
                auto script = scriptNode.as<Script>();
                deserializedEntity.AddComponent<ScriptComponent>(script.ModuleName);

                if (ScriptEngine::ModuleExists(script.ModuleName))
                {
                    auto& fieldMap = script.FieldMap;
                    auto& instanceData = ScriptEngine::GetEntityInstanceData(sceneID, entityID);
                    auto& fields = instanceData.ModuleFieldMap;
                    SetFields(fields, fieldMap, script.ModuleName);
                }
            }

            auto rigidBody2DNode = entity["RigidBody2DComponent"];
            if (rigidBody2DNode)
            {
                auto rigidBody = rigidBody2DNode.as<RigidBody2DComponent>();
                deserializedEntity.AddComponent<RigidBody2DComponent>(rigidBody);
            }

            auto boxCollider2DNode = entity["BoxCollider2DComponent"];
            if (boxCollider2DNode)
            {
                auto boxCollider = boxCollider2DNode.as<BoxCollider2DComponent>();
                deserializedEntity.AddComponent<BoxCollider2DComponent>(boxCollider.Offset, boxCollider.Size);
            }

            auto circleCollider2DNode = entity["CircleCollider2DComponent"];
            if (circleCollider2DNode)
            {
                auto circleCollider = circleCollider2DNode.as<CircleCollider2DComponent>();
                deserializedEntity.AddComponent<CircleCollider2DComponent>(circleCollider.Offset, circleCollider.Radius);
            }
        }
    }

    return true;
}

bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
{
    AB_CORE_ASSERT(false, "Not implemented yet!");
    return false;
}

} // Amber