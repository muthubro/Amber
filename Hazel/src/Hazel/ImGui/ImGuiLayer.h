#pragma once

#include "Hazel/Core.h"
#include "Hazel/Layer.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Events/KeyEvent.h"

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
	bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
	bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);
	bool OnMouseMoved(MouseMovedEvent& event);
	bool OnMouseScrolled(MouseScrolledEvent& event);
	bool OnWindowResize(WindowResizeEvent& event);
	bool OnKeyPressed(KeyPressedEvent& event);
	bool OnKeyReleased(KeyReleasedEvent& event);
	bool OnKeyTyped(KeyTypedEvent& event);

	float m_Time = 0.0f;
};

}
