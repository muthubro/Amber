#pragma once

#include "Core.h"
#include "LayerStack.h"
#include "Window.h"

#include "Events/ApplicationEvent.h"

#include "ImGui/ImGuiLayer.h"

#include "Renderer/Shader.h"

namespace Hazel {

class HAZEL_API Application
{
public:
	Application();
	virtual ~Application();

	void Run();
	void OnEvent(Event& event);

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);

	inline Window& GetWindow() { return *m_Window; }

	inline static Application& Get() { return *s_Instance; }

private:
	bool OnWindowClose(WindowCloseEvent& event);

	std::unique_ptr<Window> m_Window;
	LayerStack m_LayerStack;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;

	unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;
	std::unique_ptr<Shader> m_Shader;

	static Application* s_Instance;
};

// For use by CLIENT only
Application* CreateApplication();

}
