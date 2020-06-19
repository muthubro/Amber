#pragma once

#include <string>

#include <glm/glm.hpp>

namespace Amber
{
namespace Editor
{

enum class PropertyFlags
{
    None = 0, ColorProperty
};

void Property(const std::string& name, bool& value);
void Property(const std::string& name, float& value, float min = -1.0f, float max = 1.0f, PropertyFlags flags = PropertyFlags::None);
void Property(const std::string& name, glm::vec2& value, float min = -1.0f, float max = 1.0f, PropertyFlags flags = PropertyFlags::None);
void Property(const std::string& name, glm::vec3& value, float min = -1.0f, float max = 1.0f, PropertyFlags flags = PropertyFlags::None);
void Property(const std::string& name, glm::vec3& value, PropertyFlags flags);
void Property(const std::string& name, glm::vec4& value, float min = -1.0f, float max = 1.0f, PropertyFlags flags = PropertyFlags::None);
void Property(const std::string& name, glm::vec4& value, PropertyFlags flags);

} // Editor
} // Amber
