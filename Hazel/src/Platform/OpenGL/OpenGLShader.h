#pragma once

#include "Hazel/Renderer/Shader.h"

#include <string>

#include <glm/glm.hpp>

namespace Hazel
{

// TODO: REMOVE!
using GLenum = unsigned int;

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const std::string& filepath);
	OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource);
	~OpenGLShader();

	void Bind() const override;
	void Unbind() const override;

	inline const std::string& GetName() const override { return m_Name; }

	void UploadUniformInt(const std::string& name, const int value) const;
	void UploadUniformFloat(const std::string& name, const float value) const;
	void UploadUniformFloat2(const std::string& name, const glm::vec2& values) const;
	void UploadUniformFloat3(const std::string& name, const glm::vec3& values) const;
	void UploadUniformFloat4(const std::string& name, const glm::vec4& values) const;
	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

private:
	void ReadFile(const std::string& filepath, std::string& data);
	std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
	void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

	uint32_t m_RendererID;
	std::string m_Name;
};

}
