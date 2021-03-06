#include "abpch.h"
#include "Material.h"

namespace Amber
{

Material::Material(const Ref<Shader>& shader)
    : m_Shader(shader)
{
    AllocateStorage();

    SetFlag(MaterialFlag::DepthTest);
    SetFlag(MaterialFlag::StencilTest);
    SetFlag(MaterialFlag::Blend);
}

Material::~Material()
{
}

void Material::Bind()
{
    m_Shader->Bind();

    // Pixel shader has to be set first for uniforms that appear in both shaders
    if (m_PSUniformStorageBuffer)
        m_Shader->SetPSMaterialUniformBuffer(m_PSUniformStorageBuffer);

    if (m_VSUniformStorageBuffer)
        m_Shader->SetVSMaterialUniformBuffer(m_VSUniformStorageBuffer);

    BindTextures();
}

void Material::Reset(Ref<Shader> shader)
{
    if (shader)
        m_Shader = shader;

    m_VSUniformStorageBuffer.Clear();
    m_PSUniformStorageBuffer.Clear();
    AllocateStorage();
    for (auto instance : m_MaterialInstances)
    {
        instance->m_Material = this;
        instance->m_VSUniformStorageBuffer.Clear();
        instance->m_PSUniformStorageBuffer.Clear();
        instance->AllocateStorage();
        instance->m_Textures.clear();
    }
    m_Textures.clear();
}

void Material::AllocateStorage()
{
    if (m_Shader->HasVSMaterialUniformBuffer())
    {
        const auto& vsBuffer = m_Shader->GetVSMaterialUniformBuffer();
        m_VSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
        m_VSUniformStorageBuffer.ZeroInitialize();
    }

    if (m_Shader->HasPSMaterialUniformBuffer())
    {
        const auto& psBuffer = m_Shader->GetPSMaterialUniformBuffer();
        m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
        m_PSUniformStorageBuffer.ZeroInitialize();
    }
}

void Material::BindTextures() const
{
    for (uint32_t i = 0; i < m_Textures.size(); i++)
    {
        auto& texture = m_Textures[i];
        if (texture)
            texture->Bind(i);
    }
}

ShaderUniform* Material::FindUniform(const std::string& name) const
{
    if (m_VSUniformStorageBuffer)
    {
        const auto& uniforms = m_Shader->GetVSMaterialUniformBuffer().GetUniforms();
        for (auto uniform : uniforms)
        {
            if (uniform->GetName() == name)
                return uniform;
        }
    }

    if (m_PSUniformStorageBuffer)
    {
        const auto& uniforms = m_Shader->GetPSMaterialUniformBuffer().GetUniforms();
        for (auto uniform : uniforms)
        {
            if (uniform->GetName() == name)
                return uniform;
        }
    }

    return nullptr;
}

ShaderResource* Material::FindResource(const std::string& name) const
{
    auto& resources = m_Shader->GetResources();
    for (auto resource : resources)
    {
        if (resource->GetName() == name)
            return resource;
    }

    return nullptr;
}

Buffer& Material::GetUniformBufferTarget(ShaderUniform* uniform)
{
    switch (uniform->GetDomain())
    {
        case ShaderDomain::Vertex:  return m_VSUniformStorageBuffer;
        case ShaderDomain::Pixel:   return m_PSUniformStorageBuffer;
    }

    AB_CORE_ASSERT(false, "Invalid shader domain! Material does not support this shader type.");
    return m_VSUniformStorageBuffer;
}

const Buffer& Material::GetUniformBufferTarget(ShaderUniform* uniform) const
{
    switch (uniform->GetDomain())
    {
        case ShaderDomain::Vertex:  return m_VSUniformStorageBuffer;
        case ShaderDomain::Pixel:   return m_PSUniformStorageBuffer;
    }

    AB_CORE_ASSERT(false, "Invalid shader domain! Material does not support this shader type.");
    return m_VSUniformStorageBuffer;
}

/////////////////////////////////////////////////////////////////////
////////     MATERIAL INSTANCE     //////////////////////////////////
/////////////////////////////////////////////////////////////////////

MaterialInstance::MaterialInstance(const Ref<Material>& material, const std::string& name)
    : m_Name(name), m_Material(material)
{
    m_Material->AddMaterialInstance(this);
    AllocateStorage();
}

MaterialInstance::~MaterialInstance()
{
    m_Material->RemoveMaterialInstance(this);
}

void MaterialInstance::Bind()
{
    m_Material->GetShader()->Bind();

    if (m_PSUniformStorageBuffer)
        m_Material->GetShader()->SetPSMaterialUniformBuffer(m_PSUniformStorageBuffer);

    if (m_VSUniformStorageBuffer)
        m_Material->GetShader()->SetVSMaterialUniformBuffer(m_VSUniformStorageBuffer);

    m_Material->BindTextures();
    BindTextures();
}

void MaterialInstance::AllocateStorage()
{
    if (m_Material->GetShader()->HasVSMaterialUniformBuffer())
    {
        const auto& vsBuffer = m_Material->GetShader()->GetVSMaterialUniformBuffer();
        m_VSUniformStorageBuffer.Allocate(vsBuffer.GetSize());
        m_VSUniformStorageBuffer.Write(m_Material->GetVSUniformStorageBuffer().Data, vsBuffer.GetSize());
    }

    if (m_Material->GetShader()->HasPSMaterialUniformBuffer())
    {
        const auto& psBuffer = m_Material->GetShader()->GetPSMaterialUniformBuffer();
        m_PSUniformStorageBuffer.Allocate(psBuffer.GetSize());
        m_PSUniformStorageBuffer.Write(m_Material->GetPSUniformStorageBuffer().Data, psBuffer.GetSize());
    }
}

void MaterialInstance::BindTextures() const
{
    for (uint32_t i = 0; i < m_Textures.size(); i++)
    {
        auto& texture = m_Textures[i];
        if (texture)
            texture->Bind(i);
    }
}

Buffer& MaterialInstance::GetUniformBufferTarget(ShaderUniform* uniform)
{
    switch (uniform->GetDomain())
    {
        case ShaderDomain::Vertex:  return m_VSUniformStorageBuffer;
        case ShaderDomain::Pixel:   return m_PSUniformStorageBuffer;
    }

    AB_CORE_ASSERT(false, "Invalid shader domain! Material does not support this shader type.");
    return m_VSUniformStorageBuffer;
}

const Buffer& MaterialInstance::GetUniformBufferTarget(ShaderUniform* uniform) const
{
    switch (uniform->GetDomain())
    {
        case ShaderDomain::Vertex:  return m_VSUniformStorageBuffer;
        case ShaderDomain::Pixel:   return m_PSUniformStorageBuffer;
    }

    AB_CORE_ASSERT(false, "Invalid shader domain! Material does not support this shader type.");
    return m_VSUniformStorageBuffer;
}

void MaterialInstance::OnMaterialValueUpdated(ShaderUniform* uniform)
{
    if (m_OverriddenValues.find(uniform->GetName()) == m_OverriddenValues.end())
    {
        auto& buffer = GetUniformBufferTarget(uniform);
        auto& materialBuffer = m_Material->GetUniformBufferTarget(uniform);
        buffer.Write(materialBuffer.Data + uniform->GetOffset(), uniform->GetSize(), uniform->GetOffset());
    }
}

}