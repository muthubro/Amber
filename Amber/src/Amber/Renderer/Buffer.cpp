#include "abpch.h"
#include "Buffer.h"

#include "Amber/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Amber 
{

static uint32_t ShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:		return sizeof(float);
		case ShaderDataType::Float2:	return sizeof(float) * 2;
		case ShaderDataType::Float3:	return sizeof(float) * 3;
		case ShaderDataType::Float4:	return sizeof(float) * 4;
		case ShaderDataType::Mat3:		return sizeof(float) * 3 * 3;
		case ShaderDataType::Mat4:		return sizeof(float) * 4 * 4;
		case ShaderDataType::Int:		return sizeof(int);
		case ShaderDataType::Int2:		return sizeof(int) * 2;
		case ShaderDataType::Int3:		return sizeof(int) * 3;
		case ShaderDataType::Int4:		return sizeof(int) * 4;
		case ShaderDataType::Bool:		return sizeof(bool);
	}

	AB_CORE_ASSERT(false, "Invalid shader data type!");
	return 0;
}

BufferElement::BufferElement(ShaderDataType type, const std::string& name, bool normalized)
	: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

uint8_t BufferElement::GetComponentCount() const
{
	switch (Type)
	{
		case ShaderDataType::Float:		return 1;
		case ShaderDataType::Float2:	return 2;
		case ShaderDataType::Float3:	return 3;
		case ShaderDataType::Float4:	return 4;
		case ShaderDataType::Mat3:		return 3 * 3;
		case ShaderDataType::Mat4:		return 4 * 4;
		case ShaderDataType::Int:		return 1;
		case ShaderDataType::Int2:		return 2;
		case ShaderDataType::Int3:		return 3;
		case ShaderDataType::Int4:		return 4;
		case ShaderDataType::Bool:		return 1;
	}

	AB_CORE_ASSERT(false, "Invalid shader data type!");
	return 0;
}

BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
	: m_Elements(elements)
{
	CalculateOffsetsAndStride();
}

void BufferLayout::CalculateOffsetsAndStride() 
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

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:
			AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(size);
	}

	AB_CORE_ASSERT(false, "Unknown Renderer API");
	return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size, bool dynamic)
{
	switch (Renderer::GetAPI()) 
	{
		case RendererAPI::API::None:
			AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(vertices, size, dynamic);
	}

	AB_CORE_ASSERT(false, "Unknown Renderer API");
	return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) 
{
	switch (Renderer::GetAPI()) 
	{
		case RendererAPI::API::None:
			AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(indices, count);
	}

	AB_CORE_ASSERT(false, "Unknown Renderer API");
	return nullptr;
}

}