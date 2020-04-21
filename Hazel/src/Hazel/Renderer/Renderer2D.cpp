#include "hzpch.h"
#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Hazel/Renderer/RenderCommand.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/VertexArray.h"

namespace Hazel
{

struct Renderer2DStorage
{
	Ref<VertexArray> QuadVertexArray;
	Ref<Shader> TextureShader;
	Ref<Texture2D> WhiteTexture;
};

static Renderer2DStorage* s_Data;

void Renderer2D::Init()
{
	s_Data = new Renderer2DStorage();

	s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
	s_Data->TextureShader->SetInt("u_Texture", 0);

	s_Data->WhiteTexture = Texture2D::Create(1, 1);
	uint32_t whiteTextureData = 0xffffffff;
	s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

	s_Data->QuadVertexArray = VertexArray::Create();

	float squareVertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};
	Ref<VertexBuffer> squareVB;
	squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float2, "a_Texture"}
	});
	s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

	uint32_t squareIndices[] = { 0, 1, 2, 2, 3, 0 };
	Ref<IndexBuffer> squareIB;
	squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

	s_Data->QuadVertexArray->SetIndexBuffer(squareIB);
}

void Renderer2D::Shutdown()
{
	delete s_Data;
}

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	s_Data->TextureShader->Bind();
	s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
}

void Renderer2D::EndScene()
{
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, rotation, s_Data->WhiteTexture, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
{
	DrawQuad(position, size, rotation, s_Data->WhiteTexture, color);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, rotation, texture, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color)
{
	s_Data->TextureShader->SetFloat4("u_Color", color);
	texture->Bind();

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, position);
	transform = glm::rotate(transform, glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
	transform = glm::scale(transform, { size.x, size.y, 1.0f });
	s_Data->TextureShader->SetMat4("u_Transform", transform);

	s_Data->QuadVertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
}

}