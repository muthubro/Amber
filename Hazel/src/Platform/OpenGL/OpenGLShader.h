#pragma once

#include "Hazel/Renderer/Shader.h"

#include <string>

#include <glm/glm.hpp>

namespace Hazel
{

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource);
	~OpenGLShader();

	void Bind() const override;
	void Unbind() const override;

	void UploadUniformInt(const std::string& name, const int value) const;
	void UploadUniformFloat(const std::string& name, const float value) const;
	void UploadUniformFloat2(const std::string& name, const glm::vec2& values) const;
	void UploadUniformFloat3(const std::string& name, const glm::vec3& values) const;
	void UploadUniformFloat4(const std::string& name, const glm::vec4& values) const;
	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const;
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

private:
	uint32_t m_RendererID;
};

}
