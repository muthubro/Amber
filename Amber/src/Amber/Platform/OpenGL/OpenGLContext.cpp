#include "abpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Amber/Core/Base.h"

namespace Amber
{

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
    : m_WindowHandle(windowHandle)
{
    AB_CORE_ASSERT(windowHandle, "Window handle is null!");
    Init();
}

void OpenGLContext::Init()
{
    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    AB_CORE_ASSERT(status, "Failed to initialize Glad!");

    AB_CORE_INFO("OpenGL Info:");
    AB_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    AB_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    AB_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

    int versionMajor;
    int versionMinor;
    glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
    glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

    AB_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Amber requires at least OpenGL version 4.5!");
}

void OpenGLContext::SwapBuffers()
{
    glfwSwapBuffers(m_WindowHandle);
}

}