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

///////////////////////////////////////////////////////////////////////////////
////////////////////////////     VERTEX BUFFER     ////////////////////////////
///////////////////////////////////////////////////////////////////////////////

OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size, VertexBufferUsage usage)
    : m_Size(size)
{
    Ref<OpenGLVertexBuffer> instance = this;
    RenderCommand::Submit([instance, usage]() mutable {
        AB_PROFILE_FUNCTION();

        glCreateBuffers(1, &instance->m_RendererID);
        glNamedBufferData(instance->m_RendererID, instance->m_Size, nullptr, OpenGLUsage(usage));
    });
}

OpenGLVertexBuffer::OpenGLVertexBuffer(void* data, size_t size, VertexBufferUsage usage)
    : m_Size(size)
{
    m_LocalData = Buffer(data, size);

    Ref<OpenGLVertexBuffer> instance = this;
    RenderCommand::Submit([instance, usage]() mutable {
        AB_PROFILE_FUNCTION();
            
        glCreateBuffers(1, &instance->m_RendererID);
        glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data, OpenGLUsage(usage));
    });
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() 
{
    RendererID rendererID = m_RendererID;
    RenderCommand::Submit([rendererID]() {
        AB_PROFILE_FUNCTION();

        glDeleteBuffers(1, &rendererID);
    });
}

void OpenGLVertexBuffer::Bind() const 
{
    Ref<const OpenGLVertexBuffer> instance = this;
    RenderCommand::Submit([instance]() {
        AB_PROFILE_FUNCTION();
        
        glBindBuffer(GL_ARRAY_BUFFER, instance->m_RendererID);
    });
}

void OpenGLVertexBuffer::Unbind() const 
{
    RenderCommand::Submit([]() {
        AB_PROFILE_FUNCTION();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    });
}

void OpenGLVertexBuffer::SetData(void* buffer, size_t size, uint32_t offset)
{
    AB_PROFILE_FUNCTION();

    m_LocalData = Buffer(buffer, size);
    m_Size = size;
    
    Ref<OpenGLVertexBuffer> instance = this;
    RenderCommand::Submit([instance, offset]() {
        AB_PROFILE_FUNCTION();
        
        glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data);
    });
}

/////////////////////////////////////////////////////////////////
////////     INDEX BUFFER     ///////////////////////////////////
/////////////////////////////////////////////////////////////////

OpenGLIndexBuffer::OpenGLIndexBuffer(void* data, size_t size)
    : m_Size(size)
{
    m_LocalData = Buffer(data, size);
    
    Ref<OpenGLIndexBuffer> instance = this;
    RenderCommand::Submit([instance]() mutable {
        AB_PROFILE_FUNCTION();

        glCreateBuffers(1, &instance->m_RendererID);
        glNamedBufferData(instance->m_RendererID, instance->m_Size, instance->m_LocalData.Data, GL_STATIC_DRAW);
    }); 
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() 
{
    RendererID rendererID = m_RendererID;
    RenderCommand::Submit([rendererID]() {
        AB_PROFILE_FUNCTION();

        glDeleteBuffers(1, &rendererID);
    });
}

void OpenGLIndexBuffer::Bind() const 
{
    Ref<const OpenGLIndexBuffer> instance = this;
    RenderCommand::Submit([instance]() {
        AB_PROFILE_FUNCTION();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instance->m_RendererID);
    });
}

void OpenGLIndexBuffer::Unbind() const 
{
    RenderCommand::Submit([]() {
        AB_PROFILE_FUNCTION();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    });
}

void OpenGLIndexBuffer::SetData(void* buffer, size_t size, uint32_t offset)
{
    m_LocalData = Buffer(buffer, size);
    m_Size = size;
    
    Ref<OpenGLIndexBuffer> instance = this;
    RenderCommand::Submit([instance, offset]() {
        AB_PROFILE_FUNCTION();

        glNamedBufferSubData(instance->m_RendererID, offset, instance->m_Size, instance->m_LocalData.Data);
    });
}

}
