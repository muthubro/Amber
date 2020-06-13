#pragma once

#include "Amber.h"

namespace Amber
{

class EditorLayer : public Layer
{
public:
    EditorLayer();
    ~EditorLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Event& e) override;
    void OnUpdate(Timestep ts) override;
    void OnImGuiRender() override;

private:
    enum class PropertyFlags
    {
        None = 0, ColorProperty
    };

    Ref<Scene> m_Scene;
    Entity* m_MeshEntity = nullptr;
    Ref<Material> m_MeshMaterial;

    glm::vec2 m_ViewportBounds[2];
    bool m_AllowViewportCameraEvents = true;

    void Property(const std::string& name, bool& value);
    void Property(const std::string& name, float& value, float min = -1.0f, float max = 1.0f, PropertyFlags flags = PropertyFlags::None);
    void Property(const std::string& name, glm::vec2& value, float min = -1.0f, float max = 1.0f, PropertyFlags flags = PropertyFlags::None);
    void Property(const std::string& name, glm::vec3& value, float min = -1.0f, float max = 1.0f, PropertyFlags flags = PropertyFlags::None);
    void Property(const std::string& name, glm::vec3& value, PropertyFlags flags);
    void Property(const std::string& name, glm::vec4& value, float min = -1.0f, float max = 1.0f, PropertyFlags flags = PropertyFlags::None);
    void Property(const std::string& name, glm::vec4& value, PropertyFlags flags);
};

}

