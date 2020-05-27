#pragma once

#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Amber/Core/Buffer.h"
#include "Amber/Renderer/Shader.h"

#include "Amber/Platform/OpenGL/OpenGLShaderUniform.h"

namespace Amber
{

class OpenGLShader : public Shader
{
public:
    OpenGLShader(const std::string& filepath);
    OpenGLShader(const std::string& name, const std::string& source);
    ~OpenGLShader();

    void Bind() const override;
    void Unbind() const override;

    void SetInt(const std::string& name, int32_t value) override;
    void SetIntArray(const std::string& name, int32_t* values, uint32_t count) override;
    void SetFloat(const std::string& name, float value) override;
    void SetFloat3(const std::string& name, const glm::vec3& value) override;
    void SetFloat4(const std::string& name, const glm::vec4& value) override;
    void SetMat4(const std::string& name, const glm::mat4& value) override;

    void UploadUniformInt(const std::string& name, int32_t value);
    void UploadUniformIntArray(const std::string& name, int32_t* values, uint32_t count);
    void UploadUniformFloat(const std::string& name, float value);
    void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
    void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
    void UploadUniformFloat4(const std::string& name, const glm::vec4& values);
    void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
    void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

    const std::string& GetName() const override { return m_Name; }
    RendererID GetRendererID() const override { return m_RendererID; }

    bool HasVSMaterialUniformBuffer() const override { return (bool)m_VSMaterialUniformBuffer; }
    bool HasPSMaterialUniformBuffer() const override { return (bool)m_PSMaterialUniformBuffer; }

    const ShaderUniformBuffer& GetVSMaterialUniformBuffer() const override { return *m_VSMaterialUniformBuffer; }
    const ShaderUniformBuffer& GetPSMaterialUniformBuffer() const override { return *m_PSMaterialUniformBuffer; }

    void SetVSMaterialUniformBuffer(Buffer buffer) override;
    void SetPSMaterialUniformBuffer(Buffer buffer) override;

    const ShaderResourceList& GetResources() const override { return m_Resources; }

private:
    std::unordered_map<std::string, int32_t> m_LocationMap;
    RendererID m_RendererID = 0;
    std::string m_Name, m_AssetPath;
    bool m_IsCompute = false;

    Scope<OpenGLShaderUniformBuffer> m_VSMaterialUniformBuffer;
    Scope<OpenGLShaderUniformBuffer> m_PSMaterialUniformBuffer;
    ShaderResourceList m_Resources;
    ShaderUniformStructList m_Structs;

    std::unordered_map<GLenum, std::string> m_ShaderSource;

    void Load(const std::string& source);
    void Reload();

    std::string ReadShaderFromFile() const;
    void PreProcess(const std::string& source);
    void Parse();
    void ParseUniformStruct(const std::string& block, ShaderDomain domain);
    void ParseUniform(const std::string& statement, ShaderDomain domain);

    void CompileAndUploadShader();

    void ResolveUniforms();
    void ResolveAndSetUniforms(const Scope<OpenGLShaderUniformBuffer>& uniformBuffer, Buffer buffer);
    void ResolveAndSetUniform(OpenGLShaderUniform* uniform, Buffer buffer);
    void ResolveAndSetUniformArray(OpenGLShaderUniform* uniform, Buffer buffer);
    void ResolveAndSetUniformField(const OpenGLShaderUniform& field, byte* data, uint32_t offset);

    int32_t GetUniformLocation(const std::string& name) const;
    ShaderUniformStruct* FindStruct(const std::string& name);

    void UploadUniformInt(uint32_t location, int32_t value);
    void UploadUniformIntArray(uint32_t location, int32_t* values, uint32_t count);
    void UploadUniformFloat(uint32_t location, float value);
    void UploadUniformFloat2(uint32_t location, const glm::vec2& values);
    void UploadUniformFloat3(uint32_t location, const glm::vec3& values);
    void UploadUniformFloat4(uint32_t location, const glm::vec4& values);
    void UploadUniformMat3(uint32_t location, const glm::mat3& matrix);
    void UploadUniformMat4(uint32_t location, const glm::mat4& matrix);
    void UploadUniformMat4Array(uint32_t location, const glm::mat4& matrix, uint32_t count);

    void UploadUniformStruct(OpenGLShaderUniform* uniform, byte* buffer, uint32_t offset);

    static GLenum ShaderTypeFromString(const std::string& type);
};

}
