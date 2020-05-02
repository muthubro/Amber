#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "Amber/Renderer/VertexArray.h"

namespace Amber 
{

class RendererAPI 
{
public:
	enum class API 
	{
		None = 0, OpenGL
	};

	virtual void Init() = 0;

	virtual void SetViewport(int x, int y, uint32_t width, uint32_t height) = 0;

	virtual void EnableBlending() = 0;
	virtual void DisableBlending() = 0;
	
	virtual void EnableDepthBuffer() = 0;
	virtual void DisableDepthBuffer() = 0;

	virtual void SetClearColor(const glm::vec4& color) = 0;
	virtual void Clear() = 0;

	virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) = 0;

	static API GetAPI() { return s_API; }
	static Scope<RendererAPI> Create();

private:
	static API s_API;
};

}
