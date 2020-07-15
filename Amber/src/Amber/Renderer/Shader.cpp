#include "abpch.h"
#include "VertexArray.h"

#include "Amber/Renderer/Renderer.h"
#include "Amber/Renderer/Shader.h"

#include "Amber/Platform/OpenGL/OpenGLShader.h"

namespace Amber
{

Ref<Shader> Shader::Create(const std::string& filepath, ShaderType type)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
            return nullptr;

        case RendererAPI::API::OpenGL:
            return Ref<OpenGLShader>::Create(filepath, type);
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

Ref<Shader> Shader::CreateFromString(const std::string& name, const std::string& source)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
            return nullptr;

        case RendererAPI::API::OpenGL:
            return Ref<OpenGLShader>::Create(name, source);
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
{
    AB_CORE_ASSERT(!Exists(name), "Shader exists already!");
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

Ref<Shader> ShaderLibrary::Load(ShaderType type, const std::string& filepath)
{
    auto shader = Shader::Create(filepath, type);
    Add(shader);
    return shader;
}

Ref<Shader> ShaderLibrary::Get(const std::string& name)
{
    AB_CORE_ASSERT(Exists(name), "Shader not found!");
    return m_Shaders[name];
}

Ref<Shader> ShaderLibrary::Get(ShaderType type)
{
    switch (type)
    {
        case ShaderType::StandardStatic:    return Get("Standard Static");
        case ShaderType::StandardAnimated:  return Get("Standard Animated");
        case ShaderType::UnlitColor:        return Get("Unlit - Color");
        case ShaderType::UnlitTexture:      return Get("Unlit - Texture");
    }

    AB_CORE_ASSERT(false, "Shader not found!");
    return nullptr;
}

bool ShaderLibrary::Exists(const std::string& name) const
{
    return m_Shaders.find(name) != m_Shaders.end();
}

}