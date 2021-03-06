#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Amber/Renderer/Material.h"
#include "Amber/Renderer/Texture.h"

namespace Amber
{

class Renderer2D
{
public:

    struct QuadData
    {
        glm::mat4 Transform = glm::mat4(1.0f);
        glm::mat4 Color = { glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f) };
        glm::vec2 TexCoords[4] = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) };
        Ref<Texture2D> Texture = nullptr;
        float TilingFactor = 1.0f;

        QuadData() = default;

        QuadData(const glm::mat4& transform, const glm::mat4& color, glm::vec2 texCoords[4], const Ref<Texture2D>& texture, float tilingFactor)
            : Transform(transform), Color(color), Texture(texture), TilingFactor(tilingFactor)
        {
            memcpy(TexCoords, texCoords, sizeof(TexCoords));
        }

        QuadData(const glm::vec2& position, float rotation, const glm::vec2& scale, glm::vec4 color)
        {
            SetTransform({ position.x, position.y, 0.0f }, rotation, scale);
            SetColor(color);
        }

        QuadData(const glm::vec2& position, float rotation, const glm::vec2& scale, Ref<Texture2D> texture)
        {
            SetTransform({ position.x, position.y, 0.0f }, rotation, scale);
            Texture = texture;
        }

        void SetTransform(const glm::vec3& position, float rotation, const glm::vec2& scale)
        {
            Transform = glm::translate(glm::mat4(1.0f), position);
            if (rotation)
                Transform = glm::rotate(Transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
            Transform = glm::scale(Transform, { scale.x, scale.y, 1.0f });
        }

        void SetColor(const glm::vec4& color)
        {
            Color = { color, color, color, color };
        }

        void SetTextureBounds(const Texture2DBounds& texBounds)
        {
            if (!Texture)
                return;

            uint32_t width = Texture->GetWidth();
            uint32_t height = Texture->GetHeight();
            TexCoords[0] = { texBounds.minPos.x / (float)width, 1.0f - texBounds.maxPos.y / (float)height };
            TexCoords[1] = { texBounds.maxPos.x / (float)width, 1.0f - texBounds.maxPos.y / (float)height };
            TexCoords[2] = { texBounds.maxPos.x / (float)width, 1.0f - texBounds.minPos.y / (float)height };
            TexCoords[3] = { texBounds.minPos.x / (float)width, 1.0f - texBounds.minPos.y / (float)height };
        }
    };

    static void Init();
    static void Shutdown();

    static void BeginScene(const glm::mat4& viewProjection, bool depthTest = true);
    static void EndScene();

    static void FlushQuads();
    static void FlushLines();

    static void DrawQuad(const QuadData& data);
    static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color = glm::vec4(1.0f));

    // Stateless draw calls
    static void DrawQuad(Ref<MaterialInstance> material, const glm::mat4& transform);
    static void DrawFullscreenQuad(Ref<MaterialInstance> material);

    struct Statistics
    {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;
        uint32_t LineCount = 0;

        uint32_t GetTotalVertexCount() { return QuadCount * 4 + LineCount * 2; }
        uint32_t GetTotalIndexCount() { return QuadCount * 6 + LineCount * 2; }
    };
    static void ResetStats();
    static Statistics GetStats();

    static Ref<Texture2D> WhiteTexture();

private:
    static float GetTextureSlot(const Ref<Texture2D>& texture);
    static void SetQuadVertexData(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoord, float textureIndex, float tilingFactor);
};

}
