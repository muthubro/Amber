#pragma once

#include <string>
#include <vector>

namespace Amber
{

enum class ShaderDomain
{
    Vertex, Pixel
};
class ShaderUniformDeclaration
{
public:
    virtual const std::string& GetName() const = 0;
    virtual uint32_t GetSize() const = 0;
    virtual uint32_t GetCount() const = 0;
    virtual uint32_t GetOffset() const = 0;
    virtual ShaderDomain GetDomain() const = 0;

protected:
    virtual void SetOffset(uint32_t offset) = 0;

private:
    friend class ShaderUniformStruct;
};

typedef std::vector<ShaderUniformDeclaration*> ShaderUniformList;

class ShaderUniformStruct
{
public:
    ShaderUniformStruct(const std::string& name)
        : m_Name(name), m_Size(0), m_Offset(0) {}

    void AddField(ShaderUniformDeclaration* field)
    {
        m_Size += field->GetSize();
        uint32_t offset = 0;
        if (!m_Fields.empty())
        {
            auto prev = m_Fields.back();
            offset = prev->GetOffset() + prev->GetSize();
        }
        field->SetOffset(offset);
        m_Fields.push_back(field);
    }

    const std::string& GetName() const { return m_Name; }
    uint32_t GetSize() const { return m_Size; }

    uint32_t GetOffset() const { return m_Offset; }
    void SetOffset(uint32_t offset) { m_Offset = offset; }

    const ShaderUniformList& GetFields() const { return m_Fields; }

private:
    std::string m_Name;
    ShaderUniformList m_Fields;
    uint32_t m_Size;
    uint32_t m_Offset;
};


class ShaderUniformBufferDeclaration
{
public:
    virtual ShaderUniformDeclaration* FindUniform(const std::string& name) = 0;
    virtual void PushUniform(ShaderUniformDeclaration* uniform) = 0;

    virtual const std::string& GetName() const = 0;
    virtual const ShaderUniformList& GetUniforms() const = 0;
    virtual uint32_t GetRegister() const = 0;
    virtual uint32_t GetSize() const = 0;
    virtual ShaderDomain GetDomain() const = 0;
};

typedef std::vector<ShaderUniformBufferDeclaration*> ShaderUniformBufferList;

class ShaderUniformResourceDeclaration
{
public:
    virtual const std::string& GetName() const = 0;
    virtual uint32_t GetRegister() const = 0;
    virtual uint32_t GetCount() const = 0;
};

typedef std::vector<ShaderUniformResourceDeclaration*> ShaderUniformResourceList;

}
