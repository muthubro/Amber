#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Hazel/Core.h"

namespace Hazel 
{

class Shader 
{
public:
	Shader(const std::string& vertexSource, const std::string& fragmentSource);
	~Shader();

	void Bind() const;
	void Unbind() const;
	
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const;

private:
	uint32_t m_RendererID;
};

}
