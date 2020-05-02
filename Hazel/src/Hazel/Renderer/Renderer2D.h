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
	static void Flush();

	// Flat color (passed to 1)
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);

	// Color gradient (passed to 2)
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::mat4& color);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::mat4& color);

	// Full texture (passed to 1)
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f);

	// Texture with coordinates (passed to 1)
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], float tilingFactor = 1.0f);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], float tilingFactor = 1.0f);

	// Full tinted texture (passed to 1)
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor = 1.0f);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor = 1.0f);

	// Tinted texture with coordinates (Main implementation 1)
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], const glm::vec4& color, float tilingFactor = 1.0f);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], const glm::vec4& color, float tilingFactor = 1.0f);

	// Full gradient tinted texture (passed to 2)
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::mat4& color, float tilingFactor = 1.0f);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::mat4& color, float tilingFactor = 1.0f);

	// Gradient tinted texture with coordinates (Main implementation 2)
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], const glm::mat4& color, float tilingFactor = 1.0f);
	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], const glm::mat4& color, float tilingFactor = 1.0f);

	struct Statistics
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;

		uint32_t GetTotalVertexCount() { return QuadCount * 4; }
		uint32_t GetTotalIndexCount() { return QuadCount * 6; }
	};
	static void ResetStats();
	static Statistics GetStats();

private:
	static void FlushAndReset();
	static float GetTextureSlot(const Ref<Texture2D>& texture);
	static void SetVertexData(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoord, float textureIndex, float tilingFactor);
};

}
