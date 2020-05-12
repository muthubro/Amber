#include "abpch.h"
#include "OpenGLShaderUniform.h"

namespace Amber
{

OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, OpenGLShaderUniformDeclaration::Type type, const std::string& name, uint32_t count)
    : m_Name(name), m_Size(SizeOfUniformType(type) * count), m_Count(count), m_Domain(domain),
        m_Type(type), m_Struct(nullptr)
{
}

OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderUniformStruct* uniformStruct, const std::string& name, uint32_t count)
    : m_Name(name), m_Size(uniformStruct->GetSize() * count), m_Count(count), m_Domain(domain),
    m_Type(OpenGLShaderUniformDeclaration::Type::Struct), m_Struct(uniformStruct)
{
}

uint32_t OpenGLShaderUniformDeclaration::SizeOfUniformType(OpenGLShaderUniformDeclaration::Type type)
{
    switch (type)
    {
        case OpenGLShaderUniformDeclaration::Type::Float32:   return 4;
        case OpenGLShaderUniformDeclaration::Type::Vec2:      return 4 * 2;
        case OpenGLShaderUniformDeclaration::Type::Vec3:      return 4 * 3;
        case OpenGLShaderUniformDeclaration::Type::Vec4:      return 4 * 4;
        case OpenGLShaderUniformDeclaration::Type::Mat3:      return 4 * 3 * 3;
        case OpenGLShaderUniformDeclaration::Type::Mat4:      return 4 * 4 * 4;
        case OpenGLShaderUniformDeclaration::Type::Int32:     return 4;
    }
    return 0;
}

OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::StringToType(const std::string& type)
{
    if (type == "float")    return OpenGLShaderUniformDeclaration::Type::Float32;
    if (type == "vec2")     return OpenGLShaderUniformDeclaration::Type::Vec2;
    if (type == "vec3")     return OpenGLShaderUniformDeclaration::Type::Vec3;
    if (type == "vec4")     return OpenGLShaderUniformDeclaration::Type::Vec4;
    if (type == "mat3")     return OpenGLShaderUniformDeclaration::Type::Mat3;
    if (type == "mat4")     return OpenGLShaderUniformDeclaration::Type::Mat4;
    if (type == "int")      return OpenGLShaderUniformDeclaration::Type::Int32;

    return OpenGLShaderUniformDeclaration::Type::None;
}

std::string OpenGLShaderUniformDeclaration::TypeToString(OpenGLShaderUniformDeclaration::Type type)
{
    switch (type)
    {
        case OpenGLShaderUniformDeclaration::Type::Float32:   return "float";
        case OpenGLShaderUniformDeclaration::Type::Vec2:      return "vec2";
        case OpenGLShaderUniformDeclaration::Type::Vec3:      return "vec3";
        case OpenGLShaderUniformDeclaration::Type::Vec4:      return "vec4";
        case OpenGLShaderUniformDeclaration::Type::Mat3:      return "mat3";
        case OpenGLShaderUniformDeclaration::Type::Mat4:      return "mat4";
        case OpenGLShaderUniformDeclaration::Type::Int32:     return "int";
    }
    return "Invalid Type!";
}

void OpenGLShaderUniformDeclaration::SetOffset(uint32_t offset)
{
    if (m_Type == OpenGLShaderUniformDeclaration::Type::Struct)
        m_Struct->SetOffset(offset);

    m_Offset = offset;
}


OpenGLShaderUniformBufferDeclaration::OpenGLShaderUniformBufferDeclaration(ShaderDomain domain, const std::string& name)
    : m_Name(name), m_Domain(domain)
{
}

ShaderUniformDeclaration* OpenGLShaderUniformBufferDeclaration::FindUniform(const std::string& name)
{
    for (auto uniform : m_Uniforms)
    {
        if (uniform->GetName() == name)
            return uniform;
    }
    return nullptr;
}

void OpenGLShaderUniformBufferDeclaration::PushUniform(ShaderUniformDeclaration* uniform)
{
    uint32_t offset = 0;
    if (!m_Uniforms.empty())
    {
        auto prev = dynamic_cast<OpenGLShaderUniformDeclaration*>(m_Uniforms.back());
        offset = prev->GetOffset() + prev->GetSize();
    }
    dynamic_cast<OpenGLShaderUniformDeclaration*>(uniform)->SetOffset(offset);
    m_Size += uniform->GetSize();

    m_Uniforms.push_back(uniform);
}


OpenGLShaderUniformResourceDeclaration::OpenGLShaderUniformResourceDeclaration(OpenGLShaderUniformResourceDeclaration::Type type, const std::string& name, uint32_t count)
    : m_Name(name), m_Count(count), m_Type(type)
{
}

OpenGLShaderUniformResourceDeclaration::Type OpenGLShaderUniformResourceDeclaration::StringToType(const std::string& type)
{
    if (type == "sampler2D") return OpenGLShaderUniformResourceDeclaration::Type::Texture2D;

    return OpenGLShaderUniformResourceDeclaration::Type::None;
}

std::string OpenGLShaderUniformResourceDeclaration::TypeToString(OpenGLShaderUniformResourceDeclaration::Type type)
{
    switch (type)
    {
        case OpenGLShaderUniformResourceDeclaration::Type::Texture2D: return "sampler2D";
    }

    return "Invalid Type!";
}

}