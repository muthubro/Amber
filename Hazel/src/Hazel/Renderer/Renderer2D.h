#pragma once

#include <glm/glm.hpp>

#include "Hazel/Renderer/OrthographicCamera.h"
#include "Hazel/Renderer/Texture.h"

namespace Hazel
{

class Renderer2D
{
public:
	static void Init();
	static void Shutdown();

	static void BeginScene(const OrthographicCamera& camera);
	static void EndScene();

	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, 
		float tilingFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, 
		float tilingFactor = 1.0f, const glm::vec4& color = glm::vec4(1.0f));
};

}
