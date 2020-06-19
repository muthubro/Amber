#include "ImGuiUtils.h"

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>

namespace Amber
{
namespace Editor
{

void Property(const std::string& name, bool& value)
{
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    std::string id = "##" + name;
    ImGui::Checkbox(id.c_str(), &value);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

void Property(const std::string& name, float& value, float min, float max, PropertyFlags flags)
{
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    std::string id = "##" + name;
    ImGui::SliderFloat(id.c_str(), &value, min, max);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

void Property(const std::string& name, glm::vec2& value, float min, float max, PropertyFlags flags)
{
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    std::string id = "##" + name;
    ImGui::SliderFloat2(id.c_str(), glm::value_ptr(value), min, max);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

void Property(const std::string& name, glm::vec3& value, float min, float max, PropertyFlags flags)
{
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    std::string id = "##" + name;
    if ((uint32_t)flags & (uint32_t)PropertyFlags::ColorProperty)
        ImGui::ColorEdit3(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
    else
        ImGui::SliderFloat3(id.c_str(), glm::value_ptr(value), min, max);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

void Property(const std::string& name, glm::vec3& value, PropertyFlags flags)
{
    Property(name, value, -1.0f, 1.0f, flags);
}

void Property(const std::string& name, glm::vec4& value, float min, float max, PropertyFlags flags)
{
    ImGui::Text(name.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    std::string id = "##" + name;
    if ((uint32_t)flags & (uint32_t)PropertyFlags::ColorProperty)
        ImGui::ColorEdit4(id.c_str(), glm::value_ptr(value), ImGuiColorEditFlags_NoInputs);
    else
        ImGui::SliderFloat4(id.c_str(), glm::value_ptr(value), min, max);

    ImGui::PopItemWidth();
    ImGui::NextColumn();
}

void Property(const std::string& name, glm::vec4& value, PropertyFlags flags)
{
    Property(name, value, -1.0f, 1.0f, flags);
}

} // Editor
} // Amber