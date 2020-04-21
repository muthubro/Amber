#pragma once

#include "Hazel/Core/Core.h"
#include "Hazel/Core/Layer.h"

namespace Hazel 
{

class ImGuiLayer : public Layer 
{
public:
	ImGuiLayer();
	~ImGuiLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnImGuiRender() override;

	void Begin();
	void End();
};

}
