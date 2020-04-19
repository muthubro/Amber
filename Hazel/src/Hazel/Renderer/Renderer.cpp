#include "hzpch.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Hazel 
{

Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;

void Renderer::BeginScene(const OrthographicCamera& camera)
{
	s_SceneData->VPMatrix = camera.GetVPMatrix();
}

void Renderer::EndScene() {}

void Renderer::Submit(const std::shared_ptr<Shader>& shader,
	const std::shared_ptr<VertexArray>& vertexArray,
	const glm::mat4& transform)
{
	shader->Bind();
	std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_ViewProjection", s_SceneData->VPMatrix);
	std::dynamic_pointer_cast<OpenGLShader>(shader)->UploadUniformMat4("u_Transform", transform);
	vertexArray->Bind();
	RenderCommand::DrawIndexed(vertexArray);
}

}