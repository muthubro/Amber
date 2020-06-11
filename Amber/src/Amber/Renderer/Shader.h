#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Amber/Core/Base.h"
#include "Amber/Core/Buffer.h"

#include "Amber/Renderer/ShaderUniform.h"

namespace Amber 
{

class Shader : public RefCounted
{
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual const std::string& GetName() const = 0;
    virtual uint32_t GetRendererID() const = 0;

    virtual bool HasVSMaterialUniformBuffer() const = 0;
    virtual bool HasPSMaterialUniformBuffer() const = 0;

    virtual const ShaderUniformBuffer& GetVSMaterialUniformBuffer() const = 0;
    virtual const ShaderUniformBuffer& GetPSMaterialUniformBuffer() const = 0;

    virtual void SetVSMaterialUniformBuffer(const Buffer& buffer) = 0;
    virtual void SetPSMaterialUniformBuffer(const Buffer& buffer) = 0;

    virtual const ShaderResourceList& GetResources() const = 0;

    static Ref<Shader> Create(const std::string& filepath);
    static Ref<Shader> CreateFromString(const std::string& name, const std::string& source);

protected:

};

class ShaderLibrary : public RefCounted
{
public:
    void Add(const std::string& name, const Ref<Shader>& shader);
    void Add(const Ref<Shader>& shader);

    Ref<Shader> Load(const std::string& filepath);
    Ref<Shader> Load(const std::string& name, const std::string& filepath);

    Ref<Shader> Get(const std::string& name);

private:
    std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    
    bool Exists(const std::string& name) const;
};

}
