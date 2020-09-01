#include "abpch.h"
#include "IndexBuffer.h"

#include "Amber/Platform/OpenGL/OpenGLIndexBuffer.h"

#include "Amber/Renderer/Renderer.h"

namespace Amber
{

Ref<IndexBuffer> IndexBuffer::Create(void* data, size_t size)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return Ref<OpenGLIndexBuffer>::Create(data, size);
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}


}