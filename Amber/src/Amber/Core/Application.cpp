#include "abpch.h"
#include "Application.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "Amber/Core/Input.h"
#include "Amber/Core/Window.h"

#include "Amber/Renderer/Renderer.h"

#include "Amber/Utils/Random.h"

namespace Amber 
{

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationProps& props)
{
    AB_PROFILE_FUNCTION();

    AB_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    m_Window = Window::Create(WindowProps(props.Name, props.WindowWidth, props.WindowHeight));
    m_Window->SetEventCallback(AB_BIND_EVENT_FN(Application::OnEvent));
    m_Window->SetVSync(false);

    Random::Init();

    Renderer::Init();
    Renderer::WaitAndRender();

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
        m_Timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        if (!m_Minimized)
        {
            {
                AB_PROFILE_SCOPE("LayerStack OnUpdate");
                
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate(m_Timestep);
            }

            auto app = this;
            Renderer::Submit([app]() { app->RenderImGui(); });

            Renderer::WaitAndRender();
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
        if (event.Handled)
            break;
        (*it)->OnEvent(event);
    }
}

void Application::RenderImGui()
{
    m_ImGuiLayer->Begin();

    ImGui::Begin("Renderer");
    auto& caps = RendererAPI::GetCapabilities();
    ImGui::Text("Vendor: %s", caps.Vendor.c_str());
    ImGui::Text("Renderer: %s", caps.Renderer.c_str());
    ImGui::Text("Version: %s", caps.Version.c_str());
    ImGui::Text("Frame Time: %.2fms", m_Timestep.GetMilliseconds());
    ImGui::End();

    {
        AB_PROFILE_SCOPE("LayerStack OnImGuiRender");

        for (Layer* layer : m_LayerStack)
            layer->OnImGuiRender();
    }

    m_ImGuiLayer->End();
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

    uint32_t width = e.GetWidth(), height = e.GetHeight();
    if (width == 0 || height == 0)
    {
        m_Minimized = true;
        return false;
    }
    
    m_Minimized = false;
    RenderCommand::SetViewPort(0, 0, width, height);
    
    return false;
}

}
