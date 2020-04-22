#include "hzpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Hazel 
{

/////////////////////////////////////////////////////////////////
////////     VERTEX BUFFER     //////////////////////////////////
/////////////////////////////////////////////////////////////////

OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, uint32_t size, bool dynamic) 
{
	HZ_PROFILE_FUNCTION();

	glCreateBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() 
{
	HZ_PROFILE_FUNCTION();

	glDeleteBuffers(1, &m_RendererID);
}

void OpenGLVertexBuffer::Bind() const 
{
	HZ_PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void OpenGLVertexBuffer::Unbind() const 
{
	HZ_PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/////////////////////////////////////////////////////////////////
////////     INDEX BUFFER     ///////////////////////////////////
/////////////////////////////////////////////////////////////////

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count, bool dynamic)
	: m_Count(count) 
{
	HZ_PROFILE_FUNCTION();

	glCreateBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer() 
{
	HZ_PROFILE_FUNCTION();

	glDeleteBuffers(1, &m_RendererID);
}

void OpenGLIndexBuffer::Bind() const 
{
	HZ_PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void OpenGLIndexBuffer::Unbind() const 
{
	HZ_PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
