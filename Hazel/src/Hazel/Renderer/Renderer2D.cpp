#include "hzpch.h"
#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Hazel/Renderer/RenderCommand.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/VertexArray.h"

namespace Hazel
{

struct QuadVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
	glm::vec2 TexCoord;
	float TexIndex;
	float TilingFactor;
};

struct Renderer2DData
{
	static const uint32_t MaxQuads = 200000;
	static const uint32_t MaxVertices = MaxQuads * 4;
	static const uint32_t MaxIndices = MaxQuads * 6;
	static const uint32_t MaxTextureSlots = 32;

	Ref<VertexArray> QuadVertexArray;
	Ref<VertexBuffer> QuadVertexBuffer;
	Ref<Shader> TextureShader;
	Ref<Texture2D> WhiteTexture;

	uint32_t QuadIndexCount = 0;
	QuadVertex* QuadVertexBufferBase = nullptr;
	QuadVertex* QuadVertexBufferPtr = nullptr;

	std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
	uint32_t TextureSlotIndex = 1;

	glm::mat4 QuadVertexPositions;
	glm::vec2 QuadTextureCoordinates[4];

	Renderer2D::Statistics Stats;
};

static Renderer2DData s_Data;

void Renderer2D::Init()
{
	HZ_PROFILE_FUNCTION();

	s_Data.QuadVertexArray = VertexArray::Create();

	s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
	s_Data.QuadVertexBuffer->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float4, "a_Color" },
		{ ShaderDataType::Float2, "a_TexCoord" },
		{ ShaderDataType::Float, "a_TexIndex" },
		{ ShaderDataType::Float, "a_TilingFactor" }
	});
	s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

	s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

	uint32_t* quadIndices = new uint32_t[Renderer2DData::MaxIndices];
	for (uint32_t i = 0, offset = 0; i < Renderer2DData::MaxIndices; i += 6, offset += 4)
	{
		quadIndices[i + 0] = offset + 0;
		quadIndices[i + 1] = offset + 1;
		quadIndices[i + 2] = offset + 2;
		
		quadIndices[i + 3] = offset + 2;
		quadIndices[i + 4] = offset + 3;
		quadIndices[i + 5] = offset + 0;
	}
	Ref<IndexBuffer> quadIndexBuffer = IndexBuffer::Create(quadIndices, Renderer2DData::MaxIndices);
	s_Data.QuadVertexArray->SetIndexBuffer(quadIndexBuffer);
	delete[] quadIndices;

	s_Data.WhiteTexture = Texture2D::Create(1, 1);
	uint32_t whiteTextureData = 0xffffffff;
	s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	int32_t samplers[Renderer2DData::MaxTextureSlots];
	for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
		samplers[i] = i;

	s_Data.TextureShader = Shader::Create("assets/shaders/Texture.glsl");
	s_Data.TextureShader->Bind();
	s_Data.TextureShader->SetIntArray("u_Textures", samplers, Renderer2DData::MaxTextureSlots);

	s_Data.TextureSlots[0] = s_Data.WhiteTexture;

	s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
	s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
	s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

	s_Data.QuadTextureCoordinates[0] = { 0.0f, 0.0f };
	s_Data.QuadTextureCoordinates[1] = { 1.0f, 0.0f };
	s_Data.QuadTextureCoordinates[2] = { 1.0f, 1.0f };
	s_Data.QuadTextureCoordinates[3] = { 0.0f, 1.0f };
}

void Renderer2D::Shutdown()
{
	HZ_PROFILE_FUNCTION();
}

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	HZ_PROFILE_FUNCTION();

	s_Data.TextureShader->Bind();
	s_Data.TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());

	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

	s_Data.TextureSlotIndex = 1;
}

void Renderer2D::FlushAndReset()
{
	HZ_PROFILE_FUNCTION();

	EndScene();

	s_Data.QuadIndexCount = 0;
	s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
	s_Data.TextureSlotIndex = 1;
}

void Renderer2D::EndScene()
{
	HZ_PROFILE_FUNCTION();

	uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
	s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

	Flush();
}

void Renderer2D::Flush()
{
	HZ_PROFILE_FUNCTION();

	for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
		s_Data.TextureSlots[i]->Bind(i);

	RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
	s_Data.Stats.DrawCalls++;
}

void Renderer2D::SetVertexData(const glm::vec3& position, const glm::vec4& color, const glm::vec2& texCoord, float textureIndex, float tilingFactor)
{
	HZ_PROFILE_FUNCTION();

	s_Data.QuadVertexBufferPtr->Position = position;
	s_Data.QuadVertexBufferPtr->Color = color;
	s_Data.QuadVertexBufferPtr->TexCoord = texCoord;
	s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
	s_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
}

float Renderer2D::GetTextureSlot(const Ref<Texture2D>& texture)
{
	HZ_PROFILE_FUNCTION();

	if (!texture) return 0.0f;

	float textureIndex = -1.0f;
	for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
	{
		if (*s_Data.TextureSlots[i].get() == *texture.get())
		{
			textureIndex = (float)i;
			break;
		}
	}
	if (textureIndex == -1.0f)
	{
		if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
			FlushAndReset();

		textureIndex = (float)s_Data.TextureSlotIndex;
		s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
		s_Data.TextureSlotIndex++;
	}

	return textureIndex;
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, rotation, s_Data.WhiteTexture, s_Data.QuadTextureCoordinates, color, 1.0f);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
	DrawQuad(position, size, rotation, s_Data.WhiteTexture, s_Data.QuadTextureCoordinates, color, 1.0f);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::mat4& color)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, rotation, s_Data.WhiteTexture, s_Data.QuadTextureCoordinates, color, 1.0f);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::mat4& color)
{
	DrawQuad(position, size, rotation, s_Data.WhiteTexture, s_Data.QuadTextureCoordinates, color, 1.0f);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, s_Data.QuadTextureCoordinates, glm::vec4(1.0f), tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor)
{
	DrawQuad(position, size, rotation, texture, s_Data.QuadTextureCoordinates, glm::vec4(1.0f), tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], float tilingFactor)
{
	glm::vec2 actualTexCoords[4];
	uint32_t width = texture->GetWidth();
	uint32_t height = texture->GetHeight();
	for (uint8_t i = 0; i < 4; i++)
	{
		actualTexCoords[i][0] = texCoords[3 - i][0] / (float)width;
		actualTexCoords[i][1] = (height - texCoords[3 - i][1] - 1) / (float)height;
	}

	DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, actualTexCoords, glm::vec4(1.0f), tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], float tilingFactor)
{
	glm::vec2 actualTexCoords[4];
	uint32_t width = texture->GetWidth();
	uint32_t height = texture->GetHeight();
	for (uint8_t i = 0; i < 4; i++)
	{
		actualTexCoords[i][0] = texCoords[3 - i][0] / (float)width;
		actualTexCoords[i][1] = (height - texCoords[3 - i][1] - 1) / (float)height;
	}

	DrawQuad(position, size, rotation, texture, actualTexCoords, glm::vec4(1.0f), tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, s_Data.QuadTextureCoordinates, color, tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor)
{
	DrawQuad(position, size, rotation, texture, s_Data.QuadTextureCoordinates, color, tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], const glm::vec4& color, float tilingFactor)
{
	glm::vec2 actualTexCoords[4];
	uint32_t width = texture->GetWidth();
	uint32_t height = texture->GetHeight();
	for (uint8_t i = 0; i < 4; i++)
	{
		actualTexCoords[i][0] = texCoords[3 - i][0] / (float)width;
		actualTexCoords[i][1] = (height - texCoords[3 - i][1] - 1) / (float)height;
	}

	DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, actualTexCoords, color, tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], const glm::vec4& color, float tilingFactor)
{
	HZ_PROFILE_FUNCTION();

	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		FlushAndReset();

	float textureIndex = Renderer2D::GetTextureSlot(texture);

	glm::mat4 actualPosition;
	if (rotation)
	{
		HZ_PROFILE_SCOPE("DrawQuad Transform");

		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, position);
		transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, { size.x, size.y, 1.0f });
			
		actualPosition = transform * s_Data.QuadVertexPositions;
	}
	else
	{
		actualPosition[0] = { position.x - 0.5 * size.x, position.y - 0.5 * size.y, position.z, 1.0f };
		actualPosition[1] = { position.x + 0.5 * size.x, position.y - 0.5 * size.y, position.z, 1.0f };
		actualPosition[2] = { position.x + 0.5 * size.x, position.y + 0.5 * size.y, position.z, 1.0f };
		actualPosition[3] = { position.x - 0.5 * size.x, position.y + 0.5 * size.y, position.z, 1.0f };
	}

	const uint32_t quadVertexCount = 4;
	for (uint32_t i = 0; i < quadVertexCount; i++)
	{
		HZ_PROFILE_SCOPE("DrawQuad SetVertexData loop");

		Renderer2D::SetVertexData(actualPosition[i], color, texCoords[i], textureIndex, tilingFactor);
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::mat4& color, float tilingFactor)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, s_Data.QuadTextureCoordinates, color, tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::mat4& color, float tilingFactor)
{
	DrawQuad(position, size, rotation, texture, s_Data.QuadTextureCoordinates, color, tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], const glm::mat4& color, float tilingFactor)
{
	glm::vec2 actualTexCoords[4];
	uint32_t width = texture->GetWidth();
	uint32_t height = texture->GetHeight();
	for (uint8_t i = 0; i < 4; i++)
	{
		actualTexCoords[i][0] = texCoords[3 - i][0] / (float)width;
		actualTexCoords[i][1] = (height - texCoords[3 - i][1] - 1) / (float)height;
	}

	DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, actualTexCoords, color, tilingFactor);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec2 texCoords[4], const glm::mat4& color, float tilingFactor)
{
	HZ_PROFILE_FUNCTION();

	if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
		FlushAndReset();

	float textureIndex = Renderer2D::GetTextureSlot(texture);

	glm::mat4 actualPosition;
	if (rotation)
	{
		HZ_PROFILE_SCOPE("DrawQuad Transform");

		glm::mat4 transform(1.0f);
		transform = glm::translate(transform, position);
		transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		actualPosition = transform * s_Data.QuadVertexPositions;
	}
	else
	{
		actualPosition[0] = { position.x - 0.5 * size.x, position.y - 0.5 * size.y, position.z, 1.0f };
		actualPosition[1] = { position.x + 0.5 * size.x, position.y - 0.5 * size.y, position.z, 1.0f };
		actualPosition[2] = { position.x + 0.5 * size.x, position.y + 0.5 * size.y, position.z, 1.0f };
		actualPosition[3] = { position.x - 0.5 * size.x, position.y + 0.5 * size.y, position.z, 1.0f };
	}

	const uint32_t quadVertexCount = 4;
	for (uint8_t i = 0; i < quadVertexCount; i++)
	{
		Renderer2D::SetVertexData(actualPosition[i], color[i], texCoords[i], textureIndex, tilingFactor);
		s_Data.QuadVertexBufferPtr++;
	}

	s_Data.QuadIndexCount += 6;

	s_Data.Stats.QuadCount++;
}

void Renderer2D::ResetStats()
{
	memset(&s_Data.Stats, 0, sizeof(Statistics));
}

Renderer2D::Statistics Renderer2D::GetStats()
{
	return s_Data.Stats;
}

}