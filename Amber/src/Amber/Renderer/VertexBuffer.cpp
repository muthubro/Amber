#include "abpch.h"
#include "VertexBuffer.h"

#include "Amber/Platform/OpenGL/OpenGLVertexBuffer.h"

#include "Amber/Renderer/Renderer.h"

namespace Amber
{

VertexBufferLayout::VertexBufferLayout(const std::initializer_list<VertexBufferElement>& elements)
    : m_Elements(elements)
{
    size_t offset = 0;
    m_Stride = 0;

    for (auto& element : m_Elements)
    {
        element.Offset = offset;
        offset += element.Size;
        m_Stride += element.Size;
    }
}

Ref<VertexBuffer> VertexBuffer::Create(size_t size, VertexBufferUsage usage)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return Ref<OpenGLVertexBuffer>::Create(size, usage);
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(void* data, size_t size, VertexBufferUsage usage)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return Ref<OpenGLVertexBuffer>::Create(data, size, usage);
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

}