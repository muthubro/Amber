#pragma once

#include "Hazel/Core.h"
#include "Hazel/Layer.h"

namespace Hazel 
{

class HAZEL_API ImGuiLayer : public Layer 
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
