#pragma once

#include "Amber/Core/Window.h"

#include "Amber/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Amber 
{

class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowProps& props);
    ~WindowsWindow();

    void OnUpdate() override;

    unsigned int GetWidth() const override { return m_Data.Width; }
    unsigned int GetHeight() const override { return m_Data.Height; }

    // Window attributes
    void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    void* GetNativeWindow() const { return m_Window; }

private:
    struct WindowData
    {
        std::string Title;
        unsigned int Width, Height;
        bool VSync;

        EventCallbackFn EventCallback;
    };

    GLFWwindow* m_Window;
    Scope<GraphicsContext> m_Context;
    WindowData m_Data;

    void Init(const WindowProps& props);
    void Shutdown();
};

}