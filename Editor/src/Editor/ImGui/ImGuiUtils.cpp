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
static uint32_t s_ActivePropertyGrids = 0;

void PushID(int id)
{
    s_UIContextID++;
    ImGui::PushID(s_UIContextID + id);
    s_PropertyCount = 0;
}

void BeginPropertyGrid(uint32_t columns, int id)
{
    PushID(id);
    ImGui::Columns(columns);
    s_ActivePropertyGrids++;
}

bool Property(const std::string& label, std::string& value, bool error)
{
    bool grid = s_ActivePropertyGrids;

    if (label.empty())
    {
        ImGui::PushItemWidth(-1);
    }
    else
    {
        ImGui::Text(label.c_str());
        if (grid)
        {
            ImGui::NextColumn();
            ImGui::PushItemWidth(-1);
        }
        else
        {
            ImGui::SameLine();
        }
    }

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    char buffer[256];
    strcpy_s(buffer, value.c_str());

    if (error)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

    bool modified = false;
    if (ImGui::InputText(s_PropertyID, buffer, 256))
    {
        value = buffer;
        modified = true;
    }

    if (error)
        ImGui::PopStyleColor();

    if (grid)
    {
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }
    else if (label.empty())
    {
        ImGui::PopItemWidth();
    }

    return modified;
}

bool Property(const std::string& label, const std::string& value, bool error)
{
    bool grid = s_ActivePropertyGrids;

    if (label.empty())
    {
        ImGui::PushItemWidth(-1);
    }
    else
    {
        ImGui::Text(label.c_str());
        if (grid)
        {
            ImGui::NextColumn();
            ImGui::PushItemWidth(-1);
        }
        else
        {
            ImGui::SameLine();
        }
    }

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    char buffer[256];
    strcpy_s(buffer, value.c_str());

    if (error)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

    ImGui::InputText(s_PropertyID, buffer, 256, ImGuiInputTextFlags_ReadOnly);

    if (error)
        ImGui::PopStyleColor();

    if (grid)
    {
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }
    else if (label.empty())
    {
        ImGui::PopItemWidth();
    }

    return false;
}

bool Property(const std::string& label, char* value, bool error)
{
    bool grid = s_ActivePropertyGrids;

    if (label.empty())
    {
        ImGui::PushItemWidth(-1);
    }
    else
    {
        ImGui::Text(label.c_str());
        if (grid)
        {
            ImGui::NextColumn();
            ImGui::PushItemWidth(-1);
        }
        else
        {
            ImGui::SameLine();
        }
    }

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    if (error)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

    bool modified = false;
    if (ImGui::InputText(s_PropertyID, value, 256))
        modified = true;

    if (error)
        ImGui::PopStyleColor();

    if (grid)
    {
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }
    else if (label.empty())
    {
        ImGui::PopItemWidth();
    }

    return modified;
}

bool Property(const std::string& label, const char* value, bool error)
{
    bool grid = s_ActivePropertyGrids;

    if (label.empty())
    {
        ImGui::PushItemWidth(-1);
    }
    else
    {
        ImGui::Text(label.c_str());
        if (grid)
        {
            ImGui::NextColumn();
            ImGui::PushItemWidth(-1);
        }
        else
        {
            ImGui::SameLine();
        }
    }

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    if (error)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

    ImGui::InputText(s_PropertyID, (char*)value, 256, ImGuiInputTextFlags_ReadOnly);

    if (error)
        ImGui::PopStyleColor();

    if (grid)
    {
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }
    else if (label.empty())
    {
        ImGui::PopItemWidth();
    }

    return false;
}

bool Property(const std::string& label, bool& value)
{
    bool grid = s_ActivePropertyGrids;

    ImGui::Text(label.c_str());
    if (grid)
    {
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);
    }
    else
    {
        ImGui::SameLine();
    }

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::Checkbox(s_PropertyID, &value))
        modified = true;

    if (grid)
    {
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }

    return modified;
}

bool Property(const std::string& label, int& value, int min, int max, int speed)
{
    bool grid = s_ActivePropertyGrids;

    ImGui::Text(label.c_str());
    if (grid)
    {
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);
    }
    else
    {
        ImGui::SameLine();
    }

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::DragInt(s_PropertyID, &value, (float)speed, min, max))
        modified = true;

    if (grid)
    {
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }

    return modified;
}

bool Property(const std::string& label, uint32_t& value, uint32_t min, uint32_t max, int speed)
{
    return Property(label, *(int*)&value, (int)min, (int)max, speed);
}

bool Property(const std::string& label, float& value, float min, float max, float speed)
{
    bool grid = s_ActivePropertyGrids;

    ImGui::Text(label.c_str());
    if (grid)
    {
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);
    }
    else
    {
        ImGui::SameLine();
    }

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::DragFloat(s_PropertyID, &value, speed, min, max))
        modified = true;

    if (grid)
    {
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }

    return modified;
}

bool Property(const std::string& label, glm::vec2& value, float min, float max, float speed)
{
    bool grid = s_ActivePropertyGrids;

    ImGui::Text(label.c_str());
    if (grid)
    {
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);
    }
    else
    {
        ImGui::SameLine();
    }

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::DragFloat2(s_PropertyID, glm::value_ptr(value), speed, min, max))
        modified = true;

    if (grid)
    {
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }

    return modified;
}

bool Property(const std::string& label, glm::vec3& value, float min, float max, float speed)
{
    bool grid = s_ActivePropertyGrids;

    ImGui::Text(label.c_str());
    if (grid)
    {
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);
    }
    else
    {
        ImGui::SameLine();
    }

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::DragFloat3(s_PropertyID, glm::value_ptr(value), speed, min, max))
        modified = true;

    if (grid)
    {
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }

    return modified;
}

bool Property(const std::string& label, glm::vec3& value, PropertyFlags flags)
{
    if (flags == PropertyFlags::None)
        return Property(label, value);
        
    if ((uint32_t)flags & (uint32_t)PropertyFlags::ColorProperty)
    {
        bool grid = s_ActivePropertyGrids;

        ImGui::Text(label.c_str());
        if (grid)
        {
            ImGui::NextColumn();
            ImGui::PushItemWidth(-1);
        }
        else
        {
            ImGui::SameLine();
        }

        s_PropertyID[0] = s_PropertyID[1] = '#';
        memset(s_PropertyID + 2, 0, 14);
        sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

        bool modified = false;
        if (ImGui::ColorEdit3(s_PropertyID, glm::value_ptr(value), ImGuiColorEditFlags_NoInputs))
            modified = true;

        if (grid)
        {
            ImGui::PopItemWidth();
            ImGui::NextColumn();
        }

        return modified;
    }

    return false;
}

bool Property(const std::string& label, glm::vec4& value, float min, float max, float speed)
{
    bool grid = s_ActivePropertyGrids;

    ImGui::Text(label.c_str());
    if (grid)
    {
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);
    }
    else
    {
        ImGui::SameLine();
    }

    s_PropertyID[0] = s_PropertyID[1] = '#';
    memset(s_PropertyID + 2, 0, 14);
    sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

    bool modified = false;
    if (ImGui::DragFloat4(s_PropertyID, glm::value_ptr(value), speed, min, max))
        modified = true;

    if (grid)
    {
        ImGui::PopItemWidth();
        ImGui::NextColumn();
    }

    return modified;
}

bool Property(const std::string& label, glm::vec4& value, PropertyFlags flags)
{
    if (flags == PropertyFlags::None)
        return Property(label, value);

    if ((uint32_t)flags & (uint32_t)PropertyFlags::ColorProperty)
    {
        bool grid = s_ActivePropertyGrids;

        ImGui::Text(label.c_str());
        if (grid)
        {
            ImGui::NextColumn();
            ImGui::PushItemWidth(-1);
        }
        else
        {
            ImGui::SameLine();
        }

        s_PropertyID[0] = s_PropertyID[1] = '#';
        memset(s_PropertyID + 2, 0, 14);
        sprintf_s(s_PropertyID + 2, 14, "%x", s_PropertyCount++);

        bool modified = false;
        if (ImGui::ColorEdit4(s_PropertyID, glm::value_ptr(value), ImGuiColorEditFlags_NoInputs))
            modified = true;

        if (grid)
        {
            ImGui::PopItemWidth();
            ImGui::NextColumn();
        }

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
    if (s_ActivePropertyGrids > 0)
        s_ActivePropertyGrids--;
}

} // Editor
} // Amber