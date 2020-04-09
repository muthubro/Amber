#pragma once

#include "Hazel/Core.h"
#include "Hazel/Layer.h"

namespace Hazel {

class HAZEL_API ImGuiLayer : public Layer {
public:
	ImGuiLayer();
	~ImGuiLayer();

	void OnDetach() override;
	void OnAttach() override;
	void OnUpdate() override;
	void OnEvent(Event& event) override;

private:
	float m_Time = 0.0f;
};

}
