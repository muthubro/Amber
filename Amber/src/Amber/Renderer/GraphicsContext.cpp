#include "abpch.h"
#include "GraphicsContext.h"

#include "Amber/Renderer/Renderer.h"

#include "Amber/Platform/OpenGL/OpenGLContext.h"

namespace Amber
{

Scope<GraphicsContext> GraphicsContext::Create(void* window)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

}