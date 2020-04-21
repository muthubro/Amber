#include "hzpch.h"
#include "Renderer2D.h"

#include "Hazel/Renderer/RenderCommand.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/VertexArray.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel
{

struct Renderer2DStorage
{
	Ref<Shader> FlatColorShader;
	Ref<VertexArray> QuadVertexArray;
};

static Renderer2DStorage* s_Data;

void Renderer2D::Init()
{
	s_Data = new Renderer2DStorage();
	s_Data->FlatColorShader = Hazel::Shader::Create("assets/shaders/FlatColor.glsl");

	s_Data->QuadVertexArray = Hazel::VertexArray::Create();

	float squareVertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};
	Hazel::Ref<Hazel::VertexBuffer> squareVB;
	squareVB = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({
		{ Hazel::ShaderDataType::Float3, "a_Position" }
	});
	s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

	uint32_t squareIndices[] = { 0, 1, 2, 2, 3, 0 };
	Hazel::Ref<Hazel::IndexBuffer> squareIB;
	squareIB = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

	s_Data->QuadVertexArray->SetIndexBuffer(squareIB);
}

void Renderer2D::Shutdown()
{
	delete s_Data;
}

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
	s_Data->FlatColorShader->Bind();
	std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->UploadUniformMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
	std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->UploadUniformMat4("u_Transform", glm::mat4(1.0f));
}

void Renderer2D::EndScene()
{
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
	DrawQuad({ position.x, position.y, 0.0f }, size, color);
}

void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
	s_Data->FlatColorShader->Bind();
	std::dynamic_pointer_cast<OpenGLShader>(s_Data->FlatColorShader)->UploadUniformFloat4("u_Color", color);

	s_Data->QuadVertexArray->Bind();
	RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
}

}