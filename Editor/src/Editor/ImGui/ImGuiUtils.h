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

void PushID(int id = 0);
void BeginPropertyGrid(int id = 0);

bool Property(const std::string& label, std::string& value);
bool Property(const std::string& label, const std::string& value);

bool Property(const std::string& label, char* value);
bool Property(const std::string& label, const char* value);

bool Property(const std::string& label, bool& value);

bool Property(const std::string& label, int& value, int min = -1, int max = 1, int speed = 1);

bool Property(const std::string& label, float& value, float min = -1.0f, float max = 1.0f, float speed = 0.1f);
bool Property(const std::string& label, glm::vec2& value, float min = -1.0f, float max = 1.0f, float speed = 0.1f);

bool Property(const std::string& label, glm::vec3& value, float min = -1.0f, float max = 1.0f, float speed = 0.1f);
bool Property(const std::string& label, glm::vec3& value, PropertyFlags flags);

bool Property(const std::string& label, glm::vec4& value, float min = -1.0f, float max = 1.0f, float speed = 0.1f);
bool Property(const std::string& label, glm::vec4& value, PropertyFlags flags);

void PopID();
void EndPropertyGrid();

} // Editor
} // Amber
