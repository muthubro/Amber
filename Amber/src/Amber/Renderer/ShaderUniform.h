#pragma once

#include <string>
#include <vector>

namespace Amber
{

enum class ShaderDomain
{
    Vertex, Pixel
};
class ShaderUniform
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

typedef std::vector<ShaderUniform*> ShaderUniformList;

class ShaderUniformStruct
{
public:
    ShaderUniformStruct(const std::string& name)
        : m_Name(name), m_Size(0), m_Offset(0) {}

    void AddField(ShaderUniform* field)
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

typedef std::vector<ShaderUniformStruct*> ShaderUniformStructList;


class ShaderUniformBuffer
{
public:
    virtual ShaderUniform* FindUniform(const std::string& name) = 0;
    virtual void PushUniform(ShaderUniform* uniform) = 0;

    virtual const std::string& GetName() const = 0;
    virtual const ShaderUniformList& GetUniforms() const = 0;
    virtual uint32_t GetRegister() const = 0;
    virtual uint32_t GetSize() const = 0;
    virtual ShaderDomain GetDomain() const = 0;
};

typedef std::vector<ShaderUniformBuffer*> ShaderUniformBufferList;

class ShaderResource
{
public:
    virtual const std::string& GetName() const = 0;
    virtual uint32_t GetRegister() const = 0;
    virtual uint32_t GetCount() const = 0;

protected:
    virtual void SetRegister(uint32_t reg) = 0;
};

typedef std::vector<ShaderResource*> ShaderResourceList;

}
