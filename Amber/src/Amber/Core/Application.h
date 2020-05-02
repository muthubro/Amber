#pragma once

#include "Amber/Core/Core.h"
#include "Amber/Core/Layer.h"
#include "Amber/Core/LayerStack.h"
#include "Amber/Core/Window.h"

#include "Amber/Events/Event.h"
#include "Amber/Events/ApplicationEvent.h"

#include "Amber/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Amber 
{

class Application
{
public:
	Application();
	virtual ~Application();

	void OnEvent(Event& event);

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);

	Window& GetWindow() { return *m_Window; }

	static Application& Get() { return *s_Instance; }

private:
	Scope<Window> m_Window;
	LayerStack m_LayerStack;
	ImGuiLayer* m_ImGuiLayer;

	float m_LastFrameTime = 0.0f;
	bool m_Running = true;
	bool m_Minimized = false;

	static Application* s_Instance;

	void Run();
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);

	friend int ::main(int argc, char** argv);
};

// For use by CLIENT only
Application* CreateApplication();

}