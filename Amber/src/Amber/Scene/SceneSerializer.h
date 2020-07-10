#pragma once

#include "Amber/Scene/Scene.h"

namespace YAML
{
class Emitter;
}

namespace Amber
{

class SceneSerializer
{
public:
    SceneSerializer(const Ref<Scene>& scene);

    void Serialize(const std::string& filepath);
    void SerializeRuntime(const std::string& filepath);

    bool Deserialize(const std::string& filepath);
    bool DeserializeRuntime(const std::string& filepath);

private:
    Ref<Scene> m_Scene;

    void SerializeEntity(YAML::Emitter& out, const Entity& entity);
    void SerializeEnvironment(YAML::Emitter& out);
};

}