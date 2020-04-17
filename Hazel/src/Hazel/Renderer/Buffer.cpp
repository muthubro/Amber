#include "hzpch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Hazel {

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size, bool dynamic) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::OpenGL:
			return new OpenGLVertexBuffer(vertices, size, dynamic);

		default:
			HZ_CORE_ASSERT(false, "Unknown Renderer API");
			return nullptr;
		}
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count, bool dynamic) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::OpenGL:
			return new OpenGLIndexBuffer(indices, count, dynamic);

		default:
			HZ_CORE_ASSERT(false, "Unknown Renderer API");
			return nullptr;
		}
	}

}