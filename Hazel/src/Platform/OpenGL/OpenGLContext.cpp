#include "hzpch.h"

#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Hazel {

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle) {
	HZ_CORE_ASSERT(windowHandle, "Window handle is null!");
}

void OpenGLContext::Init() {
	glfwMakeContextCurrent(m_WindowHandle);
	int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	HZ_CORE_ASSERT(success, "Could not initialize Glad!");
}

void OpenGLContext::SwapBuffers() {
	glfwSwapBuffers(m_WindowHandle);
}

}