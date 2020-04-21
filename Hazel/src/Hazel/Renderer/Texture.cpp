#include "hzpch.h"
#include "Texture.h"

#include "Hazel/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Hazel
{
Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
		return nullptr;

	case RendererAPI::API::OpenGL:
		return CreateRef<OpenGLTexture2D>(width, height);

	default:
		HZ_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}

Ref<Texture2D> Texture2D::Create(const std::string& path)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(path);

		default:
			HZ_CORE_ASSERT(false, "Unknown Renderer API");
			return nullptr;
	}
}

}
