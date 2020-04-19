#include "hzpch.h"
#include "VertexArray.h"

#include "hzpch.h"
#include "Shader.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel
{

Shader* Shader::Create(const std::string& vertexSource, const std::string& fragmentSource)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return new OpenGLShader(vertexSource, fragmentSource);

		default:
			HZ_CORE_ASSERT(false, "Unknown Renderer API");
			return nullptr;
	}
}

}