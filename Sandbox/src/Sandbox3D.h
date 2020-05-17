#pragma once

#include <Amber.h>

class Sandbox3D : public Amber::Layer
{
public:
    Sandbox3D();

    void OnEvent(Amber::Event& e) override;
    void OnUpdate(Amber::Timestep ts) override;

    void OnImGuiRender() override;

private:
    Amber::Camera m_Camera;
};
