#include "ImGuiUtils.h"

#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>

namespace Amber
{
namespace Editor
{

static uint32_t s_UIContextID = 0;
static uint32_t s_PropertyCount = 0;
static char s_PropertyID[16];

void PushID()
{
    ImGui::PushID(s_UIContextID++);
    s_PropertyCount = 0;
}

void BeginPropertyGrid()
{
    PushID();
    ImGui::Columns(2);
}

bool Property(const std::string& label, std::string& value)
{
    if (label.empty())
    {
        ImGui::Columns();
    }
    else
    {
        ImGui::Text(label.c_str());
        ImGui::NextColumn();
    }

    ImGui::PushItemWidth(-1);

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    char buffer[256];
    strcpy_s(buffer, value.c_str());

    bool modified = false;
    if (ImGui::InputText(s_PropertyID, buffer, 256))
    {
        value = buffer;
        modified = true;
    }

    ImGui::PopItemWidth();

    if (label.empty())
        ImGui::Columns(2);
    else
        ImGui::NextColumn();

    return modified;
}

bool Property(const std::string& label, const std::string& value)
{
    if (label.empty())
    {
        ImGui::Columns();
    }
    else
    {
        ImGui::Text(label.c_str());
        ImGui::NextColumn();
    }

    ImGui::PushItemWidth(-1);

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    char buffer[256];
    strcpy_s(buffer, value.c_str());

    ImGui::InputText(s_PropertyID, buffer, 256, ImGuiInputTextFlags_ReadOnly);

    ImGui::PopItemWidth();

    if (label.empty())
        ImGui::Columns(2);
    else
        ImGui::NextColumn();

    return false;
}

bool Property(const std::string& label, char* value)
{
    if (label.empty())
    {
        ImGui::Columns();
    }
    else
    {
        ImGui::Text(label.c_str());
        ImGui::NextColumn();
    }

    ImGui::PushItemWidth(-1);

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::InputText(s_PropertyID, value, 256))
        modified = true;

    ImGui::PopItemWidth();

    if (label.empty())
        ImGui::Columns(2);
    else
        ImGui::NextColumn();

    return modified;
}

bool Property(const std::string& label, const char* value)
{
    if (label.empty())
    {
        ImGui::Columns();
    }
    else
    {
        ImGui::Text(label.c_str());
        ImGui::NextColumn();
    }

    ImGui::PushItemWidth(-1);

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    ImGui::InputText(s_PropertyID, (char*)value, 256, ImGuiInputTextFlags_ReadOnly);

    ImGui::PopItemWidth();

    if (label.empty())
        ImGui::Columns(2);
    else
        ImGui::NextColumn();

    return false;
}

bool Property(const std::string& label, bool& value)
{
    ImGui::Text(label.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::Checkbox(s_PropertyID, &value))
        modified = true;

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return modified;
}

bool Property(const std::string& label, int& value, int min, int max)
{
    ImGui::Text(label.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::DragInt(s_PropertyID, &value, 1.0f, min, max))
        modified = true;

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return modified;
}

bool Property(const std::string& label, float& value, float min, float max)
{
    ImGui::Text(label.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::DragFloat(s_PropertyID, &value, 1.0f, min, max))
        modified = true;

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return modified;
}

bool Property(const std::string& label, glm::vec2& value, float min, float max)
{
    ImGui::Text(label.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::DragFloat2(s_PropertyID, glm::value_ptr(value), 1.0f, min, max))
        modified = true;

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return modified;
}

bool Property(const std::string& label, glm::vec3& value, float min, float max)
{
    ImGui::Text(label.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::DragFloat3(s_PropertyID, glm::value_ptr(value), 1.0f, min, max))
        modified = true;

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return modified;
}

bool Property(const std::string& label, glm::vec3& value, PropertyFlags flags)
{
    if (flags == PropertyFlags::None)
        return Property(label, value, -1.0f, 1.0f);
        
    if ((uint32_t)flags & (uint32_t)PropertyFlags::ColorProperty)
    {
        ImGui::Text(label.c_str());
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);

        s_PropertyID[0] = s_PropertyID[1] = '#';
        memset(s_PropertyID + 2, 0, 14);
        sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

        bool modified = false;
        if (ImGui::ColorEdit3(s_PropertyID, glm::value_ptr(value), ImGuiColorEditFlags_NoInputs))
            modified = true;

        ImGui::PopItemWidth();
        ImGui::NextColumn();

        return modified;
    }

    return false;
}

bool Property(const std::string& label, glm::vec4& value, float min, float max)
{
    ImGui::Text(label.c_str());
    ImGui::NextColumn();
    ImGui::PushItemWidth(-1);

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::DragFloat4(s_PropertyID, glm::value_ptr(value), 1.0f, min, max))
        modified = true;

    ImGui::PopItemWidth();
    ImGui::NextColumn();

    return modified;
}

bool Property(const std::string& label, glm::vec4& value, PropertyFlags flags)
{
    if (flags == PropertyFlags::None)
        return Property(label, value, -1.0f, 1.0f);

    if ((uint32_t)flags & (uint32_t)PropertyFlags::ColorProperty)
    {
        ImGui::Text(label.c_str());
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);

        s_PropertyID[0] = s_PropertyID[1] = '#';
        memset(s_PropertyID + 2, 0, 14);
        sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

        bool modified = false;
        if (ImGui::ColorEdit4(s_PropertyID, glm::value_ptr(value), ImGuiColorEditFlags_NoInputs))
            modified = true;

        ImGui::PopItemWidth();
        ImGui::NextColumn();

        return modified;
    }

    return false;
}

void PopID()
{
    ImGui::PopID();
    s_UIContextID--;
}

void EndPropertyGrid()
{
    ImGui::Columns(1);
    PopID();
}

} // Editor
} // Amber