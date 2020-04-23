#include "hzpch.h"
#include "RendererAPI.h"

#include "Hazel/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Hazel 
{

RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

Scope<RendererAPI> RendererAPI::Create()
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateScope<OpenGLRendererAPI>();
	}

	HZ_CORE_ASSERT(false, "Unknown Renderer API");
	return nullptr;
}

}