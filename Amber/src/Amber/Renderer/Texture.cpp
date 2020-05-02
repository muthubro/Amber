#include "abpch.h"
#include "Texture.h"

#include "Amber/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLTexture.h"

namespace Amber
{
Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:
			AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(width, height);
	}

	AB_CORE_ASSERT(false, "Unknown Renderer API");
	return nullptr;
}

Ref<Texture2D> Texture2D::Create(const std::string& path)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:
			AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(path);
	}

	AB_CORE_ASSERT(false, "Unknown Renderer API");
	return nullptr;
}

}
