#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Hazel/Core.h"

namespace Hazel 
{

class Shader 
{
public:
	virtual ~Shader() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	static Shader* Create(const std::string& vertexSource, const std::string& fragmentSource);
};

}
