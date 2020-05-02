#include "abpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Amber 
{

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;
		case ShaderDataType::Mat3:		return GL_FLOAT;
		case ShaderDataType::Mat4:		return GL_FLOAT;
		case ShaderDataType::Int:		return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;
		case ShaderDataType::Bool:		return GL_BOOL;
	}

	AB_CORE_ASSERT(false, "Invalid shader data type!");
	return 0;
}

OpenGLVertexArray::OpenGLVertexArray()
{
	AB_PROFILE_FUNCTION();

	glCreateVertexArrays(1, &m_RendererID);
}

OpenGLVertexArray::~OpenGLVertexArray()
{
	AB_PROFILE_FUNCTION();

	glDeleteVertexArrays(1, &m_RendererID);
}

void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	AB_PROFILE_FUNCTION();

	AB_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer layout not set!");

	glBindVertexArray(m_RendererID);
	vertexBuffer->Bind();

	const auto& layout = vertexBuffer->GetLayout();
	for (const auto& element : layout) 
	{
		glEnableVertexAttribArray(m_VertexBufferIndex);
		glVertexAttribPointer(m_VertexBufferIndex,
			element.GetComponentCount(),
			ShaderDataTypeToOpenGLBaseType(element.Type),
			element.Normalized ? GL_TRUE : GL_FALSE,
			layout.GetStride(),
			(const void*)element.Offset);
		m_VertexBufferIndex++;
	}

	m_VertexBuffers.push_back(vertexBuffer);
}

void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
	AB_PROFILE_FUNCTION();

	glBindVertexArray(m_RendererID);
	indexBuffer->Bind();

	m_IndexBuffer = indexBuffer;
}

void OpenGLVertexArray::Bind() const
{
	AB_PROFILE_FUNCTION();

	glBindVertexArray(m_RendererID);
}

void OpenGLVertexArray::Unbind() const
{
	AB_PROFILE_FUNCTION();

	glBindVertexArray(0);
}

}