#pragma once

#include <Amber.h>

class Sandbox2D : public Amber::Layer
{
public:
    Sandbox2D();
    ~Sandbox2D() = default;

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(Amber::Timestep ts) override;
    void OnEvent(Amber::Event& e) override;
    void OnImGuiRender() override;

private:
    Amber::Ref<Amber::SceneCamera> m_Camera;
    Amber::Ref<Amber::Texture2D> m_CheckerboardTexture;
};
