#include "abpch.h"
#include "Application.h"

#include <GLFW/glfw3.h>

#include "Amber/Core/Input.h"
#include "Amber/Core/Window.h"

#include "Amber/Renderer/Renderer.h"

#include "Amber/Utils/Random.h"

namespace Amber 
{

Application* Application::s_Instance = nullptr;

Application::Application()
{
    AB_PROFILE_FUNCTION();

    AB_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    m_Window = Window::Create();
    m_Window->SetEventCallback(AB_BIND_EVENT_FN(Application::OnEvent));

    Renderer::Init();
    Random::Init();

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
}

Application::~Application()
{
    AB_PROFILE_FUNCTION();

    Renderer::Shutdown();
}

void Application::Run() 
{
    AB_PROFILE_FUNCTION();

    while (m_Running) 
    {
        AB_PROFILE_SCOPE("RunLoop");

        float time = (float)glfwGetTime();
        Timestep ts = time - m_LastFrameTime;
        m_LastFrameTime = time;

        if (!m_Minimized)
        {
            {
                AB_PROFILE_SCOPE("LayerStack OnUpdate");
                
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(ts);
            }

            m_ImGuiLayer->Begin();
            {
                AB_PROFILE_SCOPE("LayerStack OnImGuiRender");
            
                for (Layer* layer : m_LayerStack)
                    layer->OnImGuiRender();
            }
            m_ImGuiLayer->End();
        }


        m_Window->OnUpdate();
    }
}

void Application::OnEvent(Event& event) 
{
    AB_PROFILE_FUNCTION();

    EventDispatcher dispatcher(event);
    dispatcher.Dispatch<WindowCloseEvent>(AB_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(AB_BIND_EVENT_FN(Application::OnWindowResize));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) 
    {
        (*it)->OnEvent(event);
        if (event.Handled)
            break;
    }
}

void Application::PushLayer(Layer* layer) 
{
    AB_PROFILE_FUNCTION();

    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay) 
{
    AB_PROFILE_FUNCTION();

    m_LayerStack.PushOverlay(overlay);
    overlay->OnAttach();
}

bool Application::OnWindowClose(WindowCloseEvent& e) 
{
    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    AB_PROFILE_FUNCTION();

    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
        m_Minimized = true;
        return false;
    }
    
    m_Minimized = false;
    Renderer::OnWindowResize((uint32_t)e.GetWidth(), (uint32_t)e.GetHeight());
    
    return false;
}

}
