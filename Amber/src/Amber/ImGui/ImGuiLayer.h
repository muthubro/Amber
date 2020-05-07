#pragma once

#include "Amber/Core/Core.h"
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
    void OnImGuiRender() override;

    void Begin();
    void End();
};

}
