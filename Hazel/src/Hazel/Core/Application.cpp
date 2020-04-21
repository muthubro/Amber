#include "hzpch.h"
#include "Application.h"

#include <GLFW/glfw3.h>

#include "Hazel/Core/Input.h"
#include "Hazel/Core/Window.h"

#include "Hazel/Renderer/Renderer.h"

namespace Hazel 
{

Application* Application::s_Instance = nullptr;

Application::Application()
{
	HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

	m_Window = Window::Create();
	m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));

	Renderer::Init();

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);
}

Application::~Application()
{
	Renderer::Shutdown();
}

void Application::Run() 
{
	while (m_Running) 
	{
		float time = (float)glfwGetTime();
		Timestep ts = time - m_LastFrameTime;
		m_LastFrameTime = time;

		if (!m_Minimized)
		{
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(ts);
		}

		m_ImGuiLayer->Begin();
		for (Layer* layer : m_LayerStack)
			layer->OnImGuiRender();
		m_ImGuiLayer->End();

		m_Window->OnUpdate();
	}
}

void Application::OnEvent(Event& event) 
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));

	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) 
	{
		(*--it)->OnEvent(event);
		if (event.Handled)
			break;
	}
}

void Application::PushLayer(Layer* layer) 
{
	m_LayerStack.PushLayer(layer);
}

void Application::PushOverlay(Layer* overlay) 
{
	m_LayerStack.PushOverlay(overlay);
}

bool Application::OnWindowClose(WindowCloseEvent& e) 
{
	m_Running = false;
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
		m_Minimized = true;
	else
		m_Minimized = false;

	Renderer::OnWindowResize((uint32_t)e.GetWidth(), (uint32_t)e.GetHeight());
	return false;
}

}
