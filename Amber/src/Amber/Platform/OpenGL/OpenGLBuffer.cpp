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

OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size, VertexBufferUsage usage)
    : m_Size(size)
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([this, usage]()
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, m_Size, nullptr, OpenGLUsage(usage));
    });
}

OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, size_t size, VertexBufferUsage usage)
    : m_Size(size)
{
    AB_PROFILE_FUNCTION();

    m_LocalData = Buffer::Copy(data, size);
    RenderCommand::Submit([this, usage]()
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, m_Size, m_LocalData.Data, OpenGLUsage(usage));
    });
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([this]()
    {
        glDeleteBuffers(1, &m_RendererID);
    });
}

void OpenGLVertexBuffer::Bind() const 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([this]()
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    });
}

void OpenGLVertexBuffer::Unbind() const 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([this]()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    });
}

void OpenGLVertexBuffer::SetData(void* buffer, size_t size, uint32_t offset)
{
    AB_PROFILE_FUNCTION();

    m_LocalData = Buffer::Copy(buffer, size);
    m_Size = size;
    RenderCommand::Submit([this, offset]()
    {        
        glNamedBufferSubData(m_RendererID, offset, m_Size, m_LocalData.Data);
    });
}

/////////////////////////////////////////////////////////////////
////////     INDEX BUFFER     ///////////////////////////////////
/////////////////////////////////////////////////////////////////

OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, size_t size)
    : m_Size(size)
{
    AB_PROFILE_FUNCTION();

    m_LocalData = Buffer::Copy(data, size);
    RenderCommand::Submit([this]()
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, m_Size, m_LocalData.Data, GL_STATIC_DRAW);
    });
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([this]()
    {
        glDeleteBuffers(1, &m_RendererID);
    });
}

void OpenGLIndexBuffer::Bind() const 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([this]()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    });
}

void OpenGLIndexBuffer::Unbind() const 
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([this]()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    });
}

void OpenGLIndexBuffer::SetData(void* buffer, size_t size, uint32_t offset)
{
    AB_PROFILE_FUNCTION();

    m_LocalData = Buffer::Copy(buffer, size);
    m_Size = size;
    RenderCommand::Submit([this, offset]()
    {
        glNamedBufferSubData(m_RendererID, offset, m_Size, m_LocalData.Data);
    });
}

}
