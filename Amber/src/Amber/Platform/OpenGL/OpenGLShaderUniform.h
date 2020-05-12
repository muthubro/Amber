#pragma once

#include <string>

#include "Amber/Renderer/ShaderUniform.h"

namespace Amber
{

class OpenGLShaderUniformDeclaration : ShaderUniformDeclaration
{
public:
    enum class Type
    {
        None,
        Float32,
        Vec2, Vec3, Vec4,
        Mat3, Mat4,
        Int32,
        Struct
    };

    OpenGLShaderUniformDeclaration(ShaderDomain domain, Type type, const std::string& name, uint32_t count = 1);
    OpenGLShaderUniformDeclaration(ShaderDomain domain, ShaderUniformStruct* uniformStruct, const std::string& name, uint32_t count = 1);

    const std::string& GetName() const override { return m_Name; }
    uint32_t GetSize() const override { return m_Size; }
    uint32_t GetCount() const override { return m_Count; }
    uint32_t GetOffset() const override { return m_Offset; }
    ShaderDomain GetDomain() const override { return m_Domain; }

    uint32_t GetAbsoluteOffset() const { return m_Struct ? m_Struct->GetOffset() + m_Offset : m_Offset; }
    Type GetType() const { return m_Type; }
    const ShaderUniformStruct& GetShaderUniformStruct() const { AB_CORE_ASSERT(m_Struct); return *m_Struct; }
    int32_t GetLocation() const { return m_Location; }

    bool IsArray() const { return m_Count > 1; }

    static uint32_t SizeOfUniformType(Type type);
    static Type StringToType(const std::string& type);
    static std::string TypeToString(Type type);

protected:
    void SetOffset(uint32_t offset) override;

private:
    std::string m_Name;
    uint32_t m_Size;
    uint32_t m_Count;
    uint32_t m_Offset = 0;
    ShaderDomain m_Domain;

    Type m_Type;
    ShaderUniformStruct* m_Struct;
    mutable int32_t m_Location = 0;

    friend class OpenGLShaderUniformBufferDeclaration;
};

class OpenGLShaderUniformBufferDeclaration : ShaderUniformBufferDeclaration
{
public:
    OpenGLShaderUniformBufferDeclaration(ShaderDomain domain, const std::string& name);

    ShaderUniformDeclaration* FindUniform(const std::string& name) override;
    void PushUniform(ShaderUniformDeclaration* uniform) override;

    const std::string& GetName() const override { return m_Name; }
    const ShaderUniformList& GetUniforms() const override { return m_Uniforms; }
    uint32_t GetRegister() const override { return m_Register; }
    uint32_t GetSize() const override { return m_Size; }
    ShaderDomain GetDomain() const override { return m_Domain; }

private:
    std::string m_Name;
    ShaderUniformList m_Uniforms;
    uint32_t m_Register = 0;
    uint32_t m_Size = 0;
    ShaderDomain m_Domain;
};

class OpenGLShaderUniformResourceDeclaration : ShaderUniformResourceDeclaration
{
public:
    enum class Type
    {
        None, Texture2D
    };

    OpenGLShaderUniformResourceDeclaration(Type type, const std::string& name, uint32_t count = 1);

    const std::string& GetName() const override { return m_Name; }
    uint32_t GetRegister() const override { return m_Register; }
    uint32_t GetCount() const override { return m_Count; }

    Type GetType() const { return m_Type; }

    static Type StringToType(const std::string& type);
    static std::string TypeToString(Type type);

private:
    std::string m_Name;
    uint32_t m_Register = 0;
    uint32_t m_Count;

    Type m_Type;
};

}
