#pragma once

#include "Hazel/Renderer/Shader.h"

#include <string>

#include <glm/glm.hpp>

namespace Hazel
{

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const std::string& filepath);
	OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
	~OpenGLShader();

	void Bind() const override;
	void Unbind() const override;

	void SetInt(const std::string& name, int value) override;
	void SetFloat(const std::string& name, float value) override;
	void SetFloat3(const std::string& name, const glm::vec3& value) override;
	void SetFloat4(const std::string& name, const glm::vec4& value) override;
	void SetMat4(const std::string& name, const glm::mat4& value) override;

	void UploadUniformInt(const std::string& name, int value);
	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& values);
	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	inline const std::string& GetName() const override { return m_Name; }

private:
	std::unordered_map<std::string, int> m_LocationMap;
	uint32_t m_RendererID;
	std::string m_Name;

	void ReadFile(const std::string& filepath, std::string& data);
	std::unordered_map<uint32_t, std::string> PreProcess(const std::string& source);
	void Compile(const std::unordered_map<uint32_t, std::string>& shaderSources);
};

}
