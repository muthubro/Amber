#include "abpch.h"
#include "RendererAPI.h"

#include "Amber/Renderer/Renderer.h"

#include "Amber/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Amber 
{

RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

Scope<RendererAPI> RendererAPI::Create()
{
    switch (s_API) 
    {
        case RendererAPI::API::OpenGL:  return CreateScope<OpenGLRendererAPI>();
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

}