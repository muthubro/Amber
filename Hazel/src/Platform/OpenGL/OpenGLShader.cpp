#include "hzpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	const char* src = source.c_str();
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, nullptr);

	glCompileShader(shader);
	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

		std::vector<char> log(length);
		glGetShaderInfoLog(shader, length, &length, &log[0]);

		HZ_CORE_ERROR("{0}", log.data());
		HZ_CORE_ASSERT(false, "Vertex shader failed to compile!");

		glDeleteShader(shader);
	}

	return shader;
}

OpenGLShader::OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource)
{
	unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	m_RendererID = glCreateProgram();
	glAttachShader(m_RendererID, vertexShader);
	glAttachShader(m_RendererID, fragmentShader);

	glLinkProgram(m_RendererID);
	int status;
	glGetProgramiv(m_RendererID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		int length;
		glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &length);

		std::vector<char> log(length);
		glGetProgramInfoLog(m_RendererID, length, &length, &log[0]);

		HZ_CORE_ERROR("{0}", log.data());
		HZ_CORE_ASSERT(false, "OpenGLShader program failed to link!");

		glDeleteProgram(m_RendererID);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	glDetachShader(m_RendererID, vertexShader);
	glDetachShader(m_RendererID, fragmentShader);
}

OpenGLShader::~OpenGLShader()
{
	glDeleteProgram(m_RendererID);
}

void OpenGLShader::Bind() const
{
	glUseProgram(m_RendererID);
}

void OpenGLShader::Unbind() const
{
	glUseProgram(0);
}

void OpenGLShader::UploadUniformInt(const std::string& name, const int value) const
{
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniform1i(location, value);
}

void OpenGLShader::UploadUniformFloat(const std::string& name, const float value) const
{
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values) const
{
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniform2f(location, values.x, values.y);
}

void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values) const
{
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniform3f(location, values.x, values.y, values.z);
}

void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values) const
{
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniform4f(location, values.x, values.y, values.z, values.w);
}

void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const
{
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const
{
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}