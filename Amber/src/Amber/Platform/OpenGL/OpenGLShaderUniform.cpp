#include "abpch.h"
#include "OpenGLShaderUniform.h"

namespace Amber
{

OpenGLShaderUniform::OpenGLShaderUniform(ShaderDomain domain, OpenGLShaderUniform::Type type, const std::string& name, uint32_t count)
    : m_Name(name), m_Size(SizeOfUniformType(type) * count), m_Count(count), m_Domain(domain),
        m_Type(type), m_Struct(nullptr)
{
}

OpenGLShaderUniform::OpenGLShaderUniform(ShaderDomain domain, ShaderUniformStruct* uniformStruct, const std::string& name, uint32_t count)
    : m_Name(name), m_Size(uniformStruct->GetSize() * count), m_Count(count), m_Domain(domain),
        m_Type(OpenGLShaderUniform::Type::Struct), m_Struct(uniformStruct)
{
}

uint32_t OpenGLShaderUniform::SizeOfUniformType(OpenGLShaderUniform::Type type)
{
    switch (type)
    {
        case OpenGLShaderUniform::Type::Int32:     return 4;
        case OpenGLShaderUniform::Type::Float32:   return 4;
        case OpenGLShaderUniform::Type::Vec2:      return 4 * 2;
        case OpenGLShaderUniform::Type::Vec3:      return 4 * 3;
        case OpenGLShaderUniform::Type::Vec4:      return 4 * 4;
        case OpenGLShaderUniform::Type::Mat3:      return 4 * 3 * 3;
        case OpenGLShaderUniform::Type::Mat4:      return 4 * 4 * 4;
    }
    return 0;
}

OpenGLShaderUniform::Type OpenGLShaderUniform::StringToType(const std::string& type)
{
    if (type == "bool")     return OpenGLShaderUniform::Type::Int32;
    if (type == "int")      return OpenGLShaderUniform::Type::Int32;
    if (type == "float")    return OpenGLShaderUniform::Type::Float32;
    if (type == "vec2")     return OpenGLShaderUniform::Type::Vec2;
    if (type == "vec3")     return OpenGLShaderUniform::Type::Vec3;
    if (type == "vec4")     return OpenGLShaderUniform::Type::Vec4;
    if (type == "mat3")     return OpenGLShaderUniform::Type::Mat3;
    if (type == "mat4")     return OpenGLShaderUniform::Type::Mat4;

    return OpenGLShaderUniform::Type::None;
}

std::string OpenGLShaderUniform::TypeToString(OpenGLShaderUniform::Type type)
{
    switch (type)
    {
        case OpenGLShaderUniform::Type::Int32:     return "int";
        case OpenGLShaderUniform::Type::Float32:   return "float";
        case OpenGLShaderUniform::Type::Vec2:      return "vec2";
        case OpenGLShaderUniform::Type::Vec3:      return "vec3";
        case OpenGLShaderUniform::Type::Vec4:      return "vec4";
        case OpenGLShaderUniform::Type::Mat3:      return "mat3";
        case OpenGLShaderUniform::Type::Mat4:      return "mat4";
    }
    return "Invalid Type!";
}

void OpenGLShaderUniform::SetOffset(uint32_t offset)
{
    if (m_Type == OpenGLShaderUniform::Type::Struct)
        m_Struct->SetOffset(offset);

    m_Offset = offset;
}


OpenGLShaderUniformBuffer::OpenGLShaderUniformBuffer(ShaderDomain domain, const std::string& name)
    : m_Name(name), m_Domain(domain)
{
}

ShaderUniform* OpenGLShaderUniformBuffer::FindUniform(const std::string& name)
{
    for (auto uniform : m_Uniforms)
    {
        if (uniform->GetName() == name)
            return uniform;
    }
    return nullptr;
}

void OpenGLShaderUniformBuffer::PushUniform(ShaderUniform* uniform)
{
    uint32_t offset = 0;
    if (!m_Uniforms.empty())
    {
        auto prev = dynamic_cast<OpenGLShaderUniform*>(m_Uniforms.back());
        offset = prev->GetOffset() + prev->GetSize();
    }
    dynamic_cast<OpenGLShaderUniform*>(uniform)->SetOffset(offset);
    m_Size += uniform->GetSize();

    m_Uniforms.push_back(uniform);
}


OpenGLShaderResource::OpenGLShaderResource(OpenGLShaderResource::Type type, const std::string& name, uint32_t count)
    : m_Name(name), m_Count(count), m_Type(type)
{
}

OpenGLShaderResource::Type OpenGLShaderResource::StringToType(const std::string& type)
{
    if (type == "sampler2D" || type == "sampler2DMS") return OpenGLShaderResource::Type::Texture2D;
    if (type == "samplerCube") return OpenGLShaderResource::Type::TextureCube;

    return OpenGLShaderResource::Type::None;
}

std::string OpenGLShaderResource::TypeToString(OpenGLShaderResource::Type type)
{
    switch (type)
    {
        case OpenGLShaderResource::Type::Texture2D: return "sampler2D";
        case OpenGLShaderResource::Type::TextureCube: return "samplerCube";
    }

    return "Invalid Type!";
}

}