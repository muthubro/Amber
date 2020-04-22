#include "hzpch.h"
#include "OpenGLShader.h"

#include <fstream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{

static uint32_t ShaderTypeFromString(const std::string& type)
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
	HZ_PROFILE_FUNCTION();

	std::string source;
	ReadFile(filepath, source);
	auto shaderSources = PreProcess(source);
	Compile(shaderSources);

	auto lastSlashPos = filepath.find_last_of("/\\");
	lastSlashPos = lastSlashPos == std::string::npos ? 0 : lastSlashPos + 1;
	auto lastDotPos = filepath.rfind('.');
	auto count = lastDotPos == std::string::npos ?
		filepath.size() - lastSlashPos :
		lastDotPos - lastSlashPos;

	m_Name = filepath.substr(lastSlashPos, count);
}

OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource)
	: m_Name(name)
{
	HZ_PROFILE_FUNCTION();

	std::unordered_map<uint32_t, std::string> shaderSources;
	shaderSources[GL_VERTEX_SHADER] = vertexSource;
	shaderSources[GL_FRAGMENT_SHADER] = fragmentSource;
	Compile(shaderSources);
}

OpenGLShader::~OpenGLShader()
{
	HZ_PROFILE_FUNCTION();

	glDeleteProgram(m_RendererID);
}

void OpenGLShader::ReadFile(const std::string& filepath, std::string& data)
{
	HZ_PROFILE_FUNCTION();

	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if (in)
	{
		in.seekg(0, std::ios::end);
		size_t size = in.tellg();
		if (size != -1)
		{
			data.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&data[0], data.size());
		}
		else
		{
			HZ_CORE_ERROR("Shader file '{0}' could not be read!", filepath);
		}
	}
	else
	{
		HZ_CORE_ERROR("Shader file '{0}' could not be opened!", filepath);
	}
}

std::unordered_map<uint32_t, std::string> OpenGLShader::PreProcess(const std::string& source)
{
	HZ_PROFILE_FUNCTION();

	std::unordered_map<uint32_t, std::string> shaderSources;

	const char* typeToken = "#type";
	size_t tokenLength = strlen(typeToken);
	size_t pos = source.find_first_of(typeToken, 0);
	while (pos != std::string::npos)
	{
		size_t eol = source.find_first_of("\r\n", pos);
		HZ_CORE_ASSERT(eol != std::string::npos, "Shader syntax error!");
		size_t begin = source.find_first_not_of(" ", pos + tokenLength);
		std::string type = source.substr(begin, eol - begin);
		
		size_t nextLinePos = source.find_first_not_of("\r\n", eol);
		HZ_CORE_ASSERT(nextLinePos != std::string::npos, "Shader syntax error!");
		pos = source.find(typeToken, nextLinePos);

		shaderSources[ShaderTypeFromString(type)] = pos == std::string::npos ?
			source.substr(nextLinePos) :
			source.substr(nextLinePos, pos - nextLinePos);
	}

	return shaderSources;
}

void OpenGLShader::Compile(const std::unordered_map<uint32_t, std::string>& shaderSources)
{
	HZ_PROFILE_FUNCTION();

	HZ_CORE_ASSERT(shaderSources.size() <= 2, "Only 2 shaders are supported right now!");
	std::array<uint32_t, 2> shaderIDs;

	uint32_t program = glCreateProgram();

	uint8_t shaderIDIndex = 0;
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
			shaderIDs[shaderIDIndex++] = shader;
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
		for (uint8_t i = 0; i < shaderIDIndex; i++)
		{
			glDetachShader(program, shaderIDs[i]);
			glDeleteShader(shaderIDs[i]);
		}

		return;
	}
	m_RendererID = program;

	for (uint8_t i = 0; i < shaderIDIndex; i++)
	{
		glDetachShader(program, shaderIDs[i]);
		glDeleteShader(shaderIDs[i]);
	}
}

void OpenGLShader::Bind() const
{
	HZ_PROFILE_FUNCTION();

	glUseProgram(m_RendererID);
}

void OpenGLShader::Unbind() const
{
	HZ_PROFILE_FUNCTION();

	glUseProgram(0);
}

void OpenGLShader::SetInt(const std::string& name, int value)
{
	HZ_PROFILE_FUNCTION();

	UploadUniformInt(name, value);
}

void OpenGLShader::SetFloat(const std::string& name, float value)
{
	HZ_PROFILE_FUNCTION();

	UploadUniformFloat(name, value);
}

void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
{
	HZ_PROFILE_FUNCTION();

	UploadUniformFloat3(name, value);
}

void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
{
	HZ_PROFILE_FUNCTION();

	UploadUniformFloat4(name, value);
}

void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
{
	HZ_PROFILE_FUNCTION();

	UploadUniformMat4(name, value);
}

void OpenGLShader::UploadUniformInt(const std::string& name, int value)
{
	if (m_LocationMap.find(name) == m_LocationMap.end())
		m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

	int location = m_LocationMap[name];
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniform1i(location, value);
}

void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
{
	if (m_LocationMap.find(name) == m_LocationMap.end())
		m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

	int location = m_LocationMap[name];
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
{
	if (m_LocationMap.find(name) == m_LocationMap.end())
		m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

	int location = m_LocationMap[name];
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniform2f(location, values.x, values.y);
}

void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
{
	if (m_LocationMap.find(name) == m_LocationMap.end())
		m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

	int location = m_LocationMap[name];
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniform3f(location, values.x, values.y, values.z);
}

void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
{
	if (m_LocationMap.find(name) == m_LocationMap.end())
		m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

	int location = m_LocationMap[name];
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniform4f(location, values.x, values.y, values.z, values.w);
}

void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
	if (m_LocationMap.find(name) == m_LocationMap.end())
		m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

	int location = m_LocationMap[name];
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
	if (m_LocationMap.find(name) == m_LocationMap.end())
		m_LocationMap[name] = glGetUniformLocation(m_RendererID, name.c_str());

	int location = m_LocationMap[name];
	if (location == -1)
		HZ_CORE_ERROR("Uniform {0} not found!", name);
	else
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

}