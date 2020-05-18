#pragma once

#include "Amber/Core/Core.h"
#include "Amber/Core/Events/Event.h"
#include "Amber/Core/Layer.h"

namespace Amber 
{

class ImGuiLayer : public Layer 
{
public:
    ImGuiLayer();
    ~ImGuiLayer() = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnEvent(Event& e) override;

    void Begin();
    void End();
};

}
