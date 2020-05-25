#pragma once

#include "Amber.h"

class EditorLayer : public Amber::Layer
{
public:
    EditorLayer();
    ~EditorLayer();

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(Amber::Timestep ts) override;
    void OnImGuiRender() override;
    void OnEvent(Amber::Event& event) override;

private:
};
