#pragma once

#include "Amber/Core/Base.h"
#include "Amber/Core/Layer.h"
#include "Amber/Core/LayerStack.h"
#include "Amber/Core/Time.h"
#include "Amber/Core/Window.h"

#include "Amber/Core/Events/Event.h"
#include "Amber/Core/Events/ApplicationEvent.h"

#include "Amber/ImGui/ImGuiLayer.h"

namespace Amber 
{

struct ApplicationProps
{
    std::string Name;
    uint32_t WindowWidth, WindowHeight;
};

class Application
{
public:
    Application(const ApplicationProps& props = { "Amber Engine", 1280, 720 });
    virtual ~Application();

    void Run();
    void Shutdown();

    void OnEvent(Event& event);

    void RenderImGui();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    Window& GetWindow() { return *m_Window; }

    static Application& Get() { return *s_Instance; }

    static const char* GetConfiguration();

private:
    Scope<Window> m_Window;
    LayerStack m_LayerStack;
    ImGuiLayer* m_ImGuiLayer;
    Timestep m_Timestep;

    float m_LastFrameTime = 0.0f;
    bool m_Running = true;
    bool m_Minimized = false;

    static Application* s_Instance;

    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);
};

// For use by CLIENT only
Application* CreateApplication();

}
