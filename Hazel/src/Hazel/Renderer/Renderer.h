#pragma once

#include <glm/glm.hpp>

#include "OrthographicCamera.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

namespace Hazel 
{

class Renderer 
{
public:
	static void BeginScene(const OrthographicCamera& camera);
	static void EndScene();

	static void Submit(const Ref<Shader>& shader,
		const Ref<VertexArray>& vertexArray,
		const glm::mat4& transform = glm::mat4(1.0f));

	inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

private:
	struct SceneData
	{
		glm::mat4 VPMatrix;
	};

	static SceneData* s_SceneData;
};

}