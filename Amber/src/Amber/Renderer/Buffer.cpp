#include "abpch.h"
#include "Buffer.h"

#include "Amber/Renderer/Renderer.h"

#include "Amber/Platform/OpenGL/OpenGLBuffer.h"

namespace Amber 
{

Ref<VertexBuffer> VertexBuffer::Create(size_t size, VertexBufferUsage usage)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
            return nullptr;

        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLVertexBuffer>(size, usage);
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(void* data, size_t size, VertexBufferUsage usage)
{
    switch (Renderer::GetAPI()) 
    {
        case RendererAPI::API::None:
            AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
            return nullptr;

        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLVertexBuffer>(data, size, usage);
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(void* data, size_t size)
{
    switch (Renderer::GetAPI()) 
    {
        case RendererAPI::API::None:
            AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
            return nullptr;

        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLIndexBuffer>(data, size);
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

}