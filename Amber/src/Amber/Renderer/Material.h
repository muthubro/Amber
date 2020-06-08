#pragma once

#include <unordered_set>

#include "Amber/Core/Buffer.h"
#include "Amber/Core/Base.h"

#include "Amber/Renderer/Shader.h"
#include "Amber/Renderer/Texture.h"

namespace Amber
{

enum class MaterialFlag
{
    None        = BIT(0),
    DepthTest   = BIT(1),
    Blend       = BIT(2)
};

class Material
{
    friend class MaterialInstance;

public:
    Material(const Ref<Shader>& shader);

    void Bind() const;

    Ref<Shader> GetShader() { return m_Shader; }

    uint32_t GetFlags() const { return m_Flags; }
    void SetFlag(MaterialFlag flag, bool value = true) { value ? m_Flags |= (uint32_t)flag : m_Flags &= ~(uint32_t)flag; }

    template<typename T>
    void Set(const std::string& name, const T& value)
    {
        auto uniform = FindUniform(name);
        AB_CORE_ASSERT(uniform, "Could not find uniform!");

        auto& buffer = GetUniformBufferTarget(uniform);
        buffer.Write((byte*)&value, uniform->GetSize(), uniform->GetOffset());

        for (auto instance : m_MaterialInstances)
            instance->OnMaterialValueUpdated(uniform);
    }

    void Set(const std::string& name, const Ref<Texture>& texture)
    {
        auto resource = FindResource(name);
        AB_CORE_ASSERT(resource, "Could not find texture!");

        uint32_t slot = resource->GetRegister();
        if (m_Textures.size() <= slot)
            m_Textures.resize((size_t)slot + 1);
        m_Textures[slot] = texture;
    }

    void Set(const std::string& name, const Ref<Texture2D>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

    void Set(const std::string& name, const Ref<TextureCube>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

private:
    std::unordered_set<MaterialInstance*> m_MaterialInstances;

    Ref<Shader> m_Shader;
    std::vector<Ref<Texture>> m_Textures;

    Buffer m_VSUniformStorageBuffer;
    Buffer m_PSUniformStorageBuffer;

    uint32_t m_Flags;

    void AllocateStorage();
    void BindTextures() const;

    ShaderUniform* FindUniform(const std::string& name);
    ShaderResource* FindResource(const std::string& name);

    Buffer& GetVSUniformStorageBuffer() { return m_VSUniformStorageBuffer; }
    Buffer& GetPSUniformStorageBuffer() { return m_PSUniformStorageBuffer; }
    Buffer& GetUniformBufferTarget(ShaderUniform* uniform);

    void AddMaterialInstance(MaterialInstance* materialInstance) { m_MaterialInstances.insert(materialInstance); }
    void RemoveMaterialInstance(MaterialInstance* materialInstance) { m_MaterialInstances.erase(materialInstance); }
};

class MaterialInstance
{
public:
    MaterialInstance(const Ref<Material>& material);
    ~MaterialInstance();

    void Bind() const;

    Ref<Shader> GetShader() { return m_Material->GetShader(); }

    uint32_t GetFlags() const { return m_Material->GetFlags(); }
    bool GetFlag(MaterialFlag flag) const { return m_Material->GetFlags() & (uint32_t)flag; }
    void SetFlag(MaterialFlag flag, bool value = true) { m_Material->SetFlag(flag, value); }

    template<typename T>
    void Set(const std::string& name, const T& value)
    {
        auto uniform = m_Material->FindUniform(name);
        AB_CORE_ASSERT(uniform, "Could not find uniform!");

        auto& buffer = GetUniformBufferTarget(uniform);
        buffer.Write((byte*)&value, uniform->GetSize(), uniform->GetOffset());

        m_OverriddenValues.insert(name);
    }

    void Set(const std::string& name, const Ref<Texture>& texture)
    {
        auto resource = m_Material->FindResource(name);
        AB_CORE_ASSERT(resource, "Could not find texture!");

        uint32_t slot = resource->GetRegister();
        if (m_Textures.size() <= slot)
            m_Textures.resize((size_t)slot + 1);
        m_Textures[slot] = texture;
    }

    void Set(const std::string& name, const Ref<Texture2D>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

    void Set(const std::string& name, const Ref<TextureCube>& texture)
    {
        Set(name, (const Ref<Texture>&)texture);
    }

private:
    Ref<Material> m_Material;

    std::vector<Ref<Texture>> m_Textures;

    Buffer m_VSUniformStorageBuffer;
    Buffer m_PSUniformStorageBuffer;

    std::unordered_set<std::string> m_OverriddenValues;

    void AllocateStorage();
    void BindTextures() const;

    Buffer& GetUniformBufferTarget(ShaderUniform* uniform);

    void OnMaterialValueUpdated(ShaderUniform* uniform);

    friend class Material;
};

}
