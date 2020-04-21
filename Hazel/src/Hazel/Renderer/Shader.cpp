#include "hzpch.h"
#include "VertexArray.h"

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/Shader.h"

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
		return CreateRef<OpenGLShader>(filepath);

	default:
		HZ_CORE_ASSERT(false, "Unknown Renderer API");
		return nullptr;
	}
}

Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None:
			HZ_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(name, vertexSource, fragmentSource);

		default:
			HZ_CORE_ASSERT(false, "Unknown Renderer API");
			return nullptr;
	}
}

void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
{
	HZ_CORE_ASSERT(!Exists(name), "Shader exists already!");
	m_Shaders[name] = shader;
}

void ShaderLibrary::Add(const Ref<Shader>& shader)
{
	auto& name = shader->GetName();
	Add(name, shader);
}

Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
{
	auto shader = Shader::Create(filepath);
	Add(shader);
	return shader;
}

Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
{
	auto shader = Shader::Create(filepath);
	Add(name, shader);
	return shader;
}

Ref<Shader> ShaderLibrary::Get(const std::string& name)
{
	HZ_CORE_ASSERT(Exists(name), "Shader does not exist!");
	return m_Shaders[name];
}

}