#include "hzpch.h"
#include "OpenGLWindow.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"

#include <glad/glad.h>

namespace Hazel {

static bool s_GLFWInitialized = false;

Window* Window::Create(const WindowProps& props) {
	return new OpenGLWindow(props);
}

OpenGLWindow::OpenGLWindow(const WindowProps& props) {
	Init(props);
}

OpenGLWindow::~OpenGLWindow() {
	Shutdown();
}

void OpenGLWindow::Init(const WindowProps& props) {
	m_Data.Title = props.Title;
	m_Data.Width = props.Width;
	m_Data.Height = props.Height;

	HZ_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

	if (!s_GLFWInitialized) {
		int success = glfwInit();
		HZ_CORE_ASSERT(success, "Could not initialize GLFW!");
		glfwSetErrorCallback([](int error, const char* description) {
			HZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
		});

		s_GLFWInitialized = true;
	}

	m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);

	glfwMakeContextCurrent(m_Window);
	int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	HZ_CORE_ASSERT(success, "Could not initialize Glad!");

	HZ_CORE_INFO("OpenGL Info:");
	HZ_CORE_INFO("	Vendor: {0}", glGetString(GL_VENDOR));
	HZ_CORE_INFO("	Renderer: {0}", glGetString(GL_RENDERER));
	HZ_CORE_INFO("	Version: {0}", glGetString(GL_VERSION));

	glfwSetWindowUserPointer(m_Window, &m_Data);
	SetVSync(true);

	// Set GLFW callbacks
	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
		WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);
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
		
		switch (action) {
		case GLFW_PRESS: {
			KeyPressedEvent event(key, 0);
			data.EventCallback(event);
			break;
		}
		
		case GLFW_REPEAT: {
			KeyPressedEvent event(key, 1);
			data.EventCallback(event);
			break;
		}

		case GLFW_RELEASE: {
			KeyReleasedEvent event(key);
			data.EventCallback(event);
			break;
		}
		}
	});

	glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		KeyTypedEvent event(keycode);
		data.EventCallback(event);
	});

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch (action) {
		case GLFW_PRESS: {
			MouseButtonPressedEvent event(button);
			data.EventCallback(event);
			break;
		}

		case GLFW_RELEASE: {
			MouseButtonReleasedEvent event(button);
			data.EventCallback(event);
			break;
		}
		}
	});

	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		MouseMovedEvent event((float) xpos, (float) ypos);
		data.EventCallback(event);
	});

	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		MouseScrolledEvent event((float)xoffset, (float)yoffset);
		data.EventCallback(event);
	});
}

void OpenGLWindow::Shutdown() {
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void OpenGLWindow::OnUpdate() {
	glfwPollEvents();
	glfwSwapBuffers(m_Window);
}

void OpenGLWindow::SetVSync(bool enabled) {
	if (enabled) {
		glfwSwapInterval(1);
	}
	else {
		glfwSwapInterval(0);
	}

	m_Data.VSync = enabled;
}

bool OpenGLWindow::IsVSync() const{
	return m_Data.VSync;
}

}