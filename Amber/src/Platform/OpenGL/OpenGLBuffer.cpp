#include "abpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

#include "Amber/Renderer/RenderCommand.h"

namespace Amber 
{

/////////////////////////////////////////////////////////////////
////////     VERTEX BUFFER     //////////////////////////////////
/////////////////////////////////////////////////////////////////

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    });
}

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size, bool dynamic)
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

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

void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    });
}

/////////////////////////////////////////////////////////////////
////////     INDEX BUFFER     ///////////////////////////////////
/////////////////////////////////////////////////////////////////

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
    : m_Count(count) 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glCreateBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

        delete[] indices;

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

}
