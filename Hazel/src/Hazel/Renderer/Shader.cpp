#include "hzpch.h"
#include "VertexArray.h"

#include "hzpch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel
{

Ref<Shader> Shader::Create(const std::string& filepath)
{
	switch (Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
		return nullptr;

	case RendererAPI::API::OpenGL:
		return std::make_shared<OpenGLShader>(filepath);

	default:
		HZ_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}

Ref<Shader> Shader::Create(const std::string& vertexSource, const std::string& fragmentSource)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLShader>(vertexSource, fragmentSource);

		default:
			HZ_CORE_ASSERT(false, "Unknown Renderer API");
			return nullptr;
	}
}

}