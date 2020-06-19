#pragma once

#include <Amber.h>

class Sandbox3D : public Amber::Layer
{
public:
    Sandbox3D();

    void OnAttach() override;

    void OnEvent(Amber::Event& e) override;
    void OnUpdate(Amber::Timestep ts) override;

    void OnImGuiRender() override;

private:
    Amber::Ref<Amber::Scene> m_Scene;
    Amber::Entity m_CameraEntity;
    Amber::Entity m_MeshEntity;
    Amber::Ref<Amber::Material> m_MeshMaterial;
};
