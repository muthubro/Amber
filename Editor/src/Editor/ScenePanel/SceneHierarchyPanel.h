#pragma once

#include <Amber.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Amber
{
namespace Editor
{

struct SelectedSubmesh;

struct ScriptFieldEditorData
{
    std::string Name;
    float Min = 0.01f;
    float Max = 100.0f;
    float Step = 0.01f;
};

struct EntityData
{
    bool Selected = false;
    glm::vec3 Orientation = glm::vec3(0.0f);
    std::unordered_map<std::string, std::unordered_map<std::string, ScriptFieldEditorData>> ScriptFields;
};

class SceneHierarchyPanel
{
public:
    void SetContext(const Ref<Scene>& context);
    void SetOrientation(const Entity& entity, const glm::quat& orientation);
    void SetSelection(std::vector<SelectedSubmesh>& selection);

    void OnImGuiRender();

private:
    Ref<Scene> m_Context = nullptr;
    std::vector<SelectedSubmesh>* m_SelectionContext = nullptr;
    std::unordered_map<uint32_t, EntityData> m_EntityMap;

    glm::vec3 m_PrevOrientation = glm::vec3(0.0f);

    bool DrawEntityNode(Entity& entity, int32_t index);
    void DrawComponents(Entity& entity);
};

} // Editor
} // Amber