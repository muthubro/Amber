#include "abpch.h"
#include "WindowsWindow.h"

#include "Amber/Core/Input.h"

#include "Amber/Core/Events/ApplicationEvent.h"
#include "Amber/Core/Events/KeyEvent.h"
#include "Amber/Core/Events/MouseEvent.h"

#include "Amber/Renderer/Renderer.h"

#include <ImGui/imgui.h>

namespace Amber 
{

static uint8_t s_GLFWWindowCount = 0;

WindowsWindow::WindowsWindow(const WindowProps& props)
{
    AB_PROFILE_FUNCTION();

    Init(props);
}

WindowsWindow::~WindowsWindow()
{
    AB_PROFILE_FUNCTION();

    Shutdown();
}

void WindowsWindow::Init(const WindowProps& props)
{
    AB_PROFILE_FUNCTION();

    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    AB_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (s_GLFWWindowCount == 0)
    {
        AB_PROFILE_SCOPE("glfwInit");
        int success = glfwInit();
        AB_CORE_ASSERT(success, "Could not initialize GLFW!");
        glfwSetErrorCallback([](int error, const char* description) {
            AB_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
        });
    }

    {
        AB_PROFILE_SCOPE("glfwCreateWindow");

#ifdef AB_DEBUG
        if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
        s_GLFWWindowCount++;
    }

    m_Context = GraphicsContext::Create(m_Window);
    glfwMaximizeWindow(m_Window);

    glfwSetWindowUserPointer(m_Window, &m_Data);

    // Set GLFW callbacks
    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;

        WindowResizeEvent event(width, height);
        data.EventCallback(event);
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        WindowCloseEvent event;
        data.EventCallback(event);
    });

    glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action)
        {
            case GLFW_PRESS:
            {
                KeyPressedEvent event(static_cast<KeyCode>(key), 0);
                data.EventCallback(event);
                break;
            }

            case GLFW_REPEAT:
            {
                KeyPressedEvent event(static_cast<KeyCode>(key), 1);
                data.EventCallback(event);
                break;
            }

            case GLFW_RELEASE:
            {
                KeyReleasedEvent event(static_cast<KeyCode>(key));
                data.EventCallback(event);
                break;
            }
        }
    });

    glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        KeyTypedEvent event(static_cast<KeyCode>(keycode));
        data.EventCallback(event);
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action)
        {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(static_cast<MouseCode>(button));
                data.EventCallback(event);
                break;
            }

            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
                data.EventCallback(event);
                break;
            }
        }
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseMovedEvent event((float)xpos, (float)ypos);
        data.EventCallback(event);
    });

    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseScrolledEvent event((float)xoffset, (float)yoffset);
        data.EventCallback(event);
    });

    m_ImGuiMouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_ImGuiMouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);   // FIXME: GLFW doesn't have this.
    m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);
    m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
    m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);  // FIXME: GLFW doesn't have this.
    m_ImGuiMouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);  // FIXME: GLFW doesn't have this.
    m_ImGuiMouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

    int width, height;
    glfwGetWindowSize(m_Window, &width, &height);
    m_Data.Width = width;
    m_Data.Height = height;
}

void WindowsWindow::Shutdown()
{
    AB_PROFILE_FUNCTION();

    glfwDestroyWindow(m_Window);
    s_GLFWWindowCount--;

    if (s_GLFWWindowCount == 0)
    {
        AB_CORE_INFO("Terminating GLFW");
        glfwTerminate();
    }
}

void WindowsWindow::OnUpdate()
{
    AB_PROFILE_FUNCTION();

    glfwPollEvents();
    m_Context->SwapBuffers();

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    glfwSetCursor(m_Window, m_ImGuiMouseCursors[imgui_cursor] ? m_ImGuiMouseCursors[imgui_cursor] : m_ImGuiMouseCursors[ImGuiMouseCursor_Arrow]);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void WindowsWindow::SetTitle(const std::string& title)
{
    m_Data.Title = title;
    glfwSetWindowTitle(m_Window, title.c_str());
}

void WindowsWindow::SetVSync(bool enabled)
{
    AB_PROFILE_FUNCTION();

    if (enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

    m_Data.VSync = enabled;
}

bool WindowsWindow::IsVSync() const
{
    return m_Data.VSync;
}

}