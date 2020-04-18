#include "hzpch.h"
#include "Shader.h"

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

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource) 
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
		HZ_CORE_ASSERT(false, "Shader program failed to link!");

		glDeleteProgram(m_RendererID);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	glDetachShader(m_RendererID, vertexShader);
	glDetachShader(m_RendererID, fragmentShader);
}

Shader::~Shader() 
{
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const 
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const 
{
	glUseProgram(0);
}

void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const
{
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}