#include "hzpch.h"
#include "OpenGLShader.h"

#include <fstream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{

static GLenum ShaderTypeFromString(const std::string& type)
{
	if (type == "vertex")
	{
		return GL_VERTEX_SHADER;
	}
	else if (type == "fragment" || type == "pixel")
	{
		return GL_FRAGMENT_SHADER;
	}

	HZ_CORE_ASSERT(false, "Invalid shader type '{0}'", type);
	return 0;
}

OpenGLShader::OpenGLShader(const std::string& filepath)
{
	std::string source;
	ReadFile(filepath, source);
	auto shaderSources = PreProcess(source);
	Compile(shaderSources);
}

OpenGLShader::OpenGLShader(const std::string& vertexSource, const std::string& fragmentSource)
{
	std::unordered_map<GLenum, std::string> shaderSources;
	shaderSources[GL_VERTEX_SHADER] = vertexSource;
	shaderSources[GL_FRAGMENT_SHADER] = fragmentSource;

	Compile(shaderSources);
}

OpenGLShader::~OpenGLShader()
{
	glDeleteProgram(m_RendererID);
}

void OpenGLShader::ReadFile(const std::string& filepath, std::string& data)
{
	std::ifstream in(filepath, std::ios::in, std::ios::binary);
	if (in)
	{
		in.seekg(0, std::ios::end);
		data.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&data[0], data.size());
		in.close();
	}
	else
	{
		HZ_CORE_ERROR("Shader file '{0}' could not be opened!", filepath);
	}
}

std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
{
	std::unordered_map<GLenum, std::string> shaderSources;

	const char* typeToken = "#type";
	size_t tokenLength = strlen(typeToken);
	size_t pos = source.find_first_of(typeToken, 0);
	while (pos != std::string::npos)
	{
		size_t eol = source.find_first_of("\r\n", pos);
		HZ_CORE_ASSERT(eol != std::string::npos, "Shader syntax error");
		size_t begin = source.find_first_not_of(" ", pos + tokenLength);
		std::string type = source.substr(begin, eol - begin);
		
		size_t nextLinePos = source.find_first_not_of("\r\n", eol);
		pos = source.find(typeToken, nextLinePos);
		shaderSources[ShaderTypeFromString(type)] = source.substr(
			nextLinePos, 
			pos - (nextLinePos == std::string::npos ? source.size()-1 : nextLinePos));
	}

	return shaderSources;
}

void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
{
	std::vector<uint32_t> shaders;
	shaders.reserve(shaderSources.size());

	uint32_t program = glCreateProgram();

	for (auto [type, source] : shaderSources)
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
			HZ_CORE_ASSERT(false, "Shader failed to compile!");

			glDeleteShader(shader);

			break;
		}
		else
		{
			glAttachShader(program, shader);
			shaders.push_back(shader);
		}
	}

	glLinkProgram(program);
	int status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		int length;
		glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &length);

		std::vector<char> log(length);
		glGetProgramInfoLog(m_RendererID, length, &length, &log[0]);

		HZ_CORE_ERROR("{0}", log.data());
		HZ_CORE_ASSERT(false, "OpenGLShader program failed to link!");

		glDeleteProgram(program);
		for (auto shader : shaders)
			glDeleteShader(shader);
	}
	m_RendererID = program;

	for (auto shader : shaders)
		glDetachShader(program, shader);
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