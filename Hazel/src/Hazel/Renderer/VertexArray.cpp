#include "hzpch.h"
#include "VertexArray.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Hazel 
{

Ref<VertexArray> VertexArray::Create()
{
	switch (Renderer::GetAPI()) 
	{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexArray>();

		default:
			HZ_CORE_ASSERT(false, "Unknown Renderer API");
			return nullptr;
	}
}

}