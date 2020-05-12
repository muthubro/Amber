#include "abpch.h"
#include "VertexArray.h"

#include "Amber/Renderer/Renderer.h"

#include "Amber/Platform/OpenGL/OpenGLVertexArray.h"

namespace Amber 
{

Ref<VertexArray> VertexArray::Create()
{
    switch (Renderer::GetAPI()) 
    {
        case RendererAPI::API::None:
            AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
            return nullptr;

        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLVertexArray>();
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

}