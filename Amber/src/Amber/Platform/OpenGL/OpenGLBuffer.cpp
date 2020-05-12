#include "abpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

#include "Amber/Renderer/RenderCommand.h"

namespace Amber 
{

static GLenum OpenGLUsage(VertexBufferUsage usage)
{
    switch (usage)
    {
        case VertexBufferUsage::Static:  return GL_STATIC_DRAW;
        case VertexBufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
    }

    AB_CORE_ASSERT("Unknown vertex buffer usage!");
    return 0;
}

/////////////////////////////////////////////////////////////////
////////     VERTEX BUFFER     //////////////////////////////////
/////////////////////////////////////////////////////////////////

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage)
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, OpenGLUsage(usage));
    });
}

OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage)
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, data, OpenGLUsage(usage));

        AB_CORE_TRACE("Creating vertex buffer {}", m_RendererID);
    });
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glDeleteBuffers(1, &m_RendererID);
    });
}

void OpenGLVertexBuffer::Bind() const 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        AB_CORE_TRACE("Binding vertex buffer {}", m_RendererID);
    });
}

void OpenGLVertexBuffer::Unbind() const 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    });
}

void OpenGLVertexBuffer::SetData(void* buffer, uint32_t size, uint32_t offset)
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, buffer);
    });
}

/////////////////////////////////////////////////////////////////
////////     INDEX BUFFER     ///////////////////////////////////
/////////////////////////////////////////////////////////////////

OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, uint32_t count)
    : m_Count(count) 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);

        delete[] data;

        AB_CORE_TRACE("Creating index buffer {}", m_RendererID);
    });
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glDeleteBuffers(1, &m_RendererID);
    });
}

void OpenGLIndexBuffer::Bind() const 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);

        AB_CORE_TRACE("Binding index buffer {}", m_RendererID);
    });
}

void OpenGLIndexBuffer::Unbind() const 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    });
}

void OpenGLIndexBuffer::SetData(void* buffer, uint32_t size, uint32_t offset)
{
}

}
