#include "Sandbox2D.h"

#include "Hazel.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox 2D"), m_CameraController(1280.0f/720.0f, true),
		m_SquareParticleSystem(100000, Hazel::ParticleType::Square),
		m_CircleParticleSystem(100000, Hazel::ParticleType::Circle) {}

void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
	
	m_ParticleProperties.Position =			 { 0.0f, 0.0f };
	m_ParticleProperties.Velocity =			 { 0.0f, 0.0f };
	m_ParticleProperties.VelocityVariation = { 3.0f, 3.0f };
	m_ParticleProperties.ColorBegin =		 { 1.0f, 0.6f, 0.0f, 1.0f };
	m_ParticleProperties.ColorEnd =			 { 1.0f, 0.0f, 0.0f, 1.0f };
	m_ParticleProperties.SizeBegin =		 0.2f;
	m_ParticleProperties.SizeVariation =	 0.1f;
	m_ParticleProperties.SizeEnd =			 0.0f;
	m_ParticleProperties.Lifetime =			 1.5f;

	Hazel::RenderCommand::EnableBlending();
}

void Sandbox2D::OnDetach() 
{
	HZ_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	HZ_PROFILE_FUNCTION();
	// Update
	{
		HZ_PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	}

	// Render
	{
		HZ_PROFILE_SCOPE("Renderer Prep");
		//Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Hazel::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
		Hazel::RenderCommand::Clear();
	}

	{
		HZ_PROFILE_SCOPE("Renderer Draw");

		Hazel::Renderer2D::ResetStats();

		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		//Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, 0.0f, {
		//	{ 0.0f, 0.0f, 0.0f, 1.0f },
		//	{ 1.0f, 0.0f, 0.0f, 1.0f },
		//	{ 1.0f, 1.0f, 0.0f, 1.0f },
		//	{ 0.0f, 1.0f, 0.0f, 1.0f },
		//	});
		//Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, 30.0f, { 0.2f, 0.3f, 0.8f, 1.0f });
		//Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, 0.0f, m_CheckerboardTexture, 10.0f);
		//Hazel::Renderer2D::DrawQuad({ -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, rotation, m_CheckerboardTexture, 20.0f, {
		//	{ 1.0f, 0.6f, 0.3f, 1.0f },
		//	{ 0.6f, 0.3f, 0.0f, 1.0f },
		//	{ 0.3f, 0.0f, 1.0f, 1.0f },
		//	{ 0.0f, 1.0f, 0.6f, 1.0f },
		//	});
		//Hazel::Renderer2D::EndScene();

		//Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		//for (float y = -5.0f; y < 5.0f; y += 0.5f)
		//{
		//	for (float x = -5.0f; x < 5.0f; x += 0.5f)
		//	{
		//		glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
		//		Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, 0.0f, color);
		//	}
		//}
		//Hazel::Renderer2D::EndScene();
	}

	if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Hazel::Input::GetMousePosition();
		m_ParticleProperties.Position = m_CameraController.GetWorldCoordinates({ x, y, 0.0f });

		if (m_SquareParticles)
			m_SquareParticleSystem.Emit(m_ParticleProperties, 10);
		else
			m_CircleParticleSystem.Emit(m_ParticleProperties, 10);
	}

	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Hazel::RenderCommand::EnableDepthBuffer();
	m_SquareParticleSystem.OnUpdate(ts);
	Hazel::Renderer2D::EndScene();

	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Hazel::RenderCommand::DisableDepthBuffer();
	m_CircleParticleSystem.OnUpdate(ts);
	Hazel::Renderer2D::EndScene();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = Hazel::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::NewLine();

	ImGui::ColorEdit4("Particle start color", glm::value_ptr(m_ParticleProperties.ColorBegin));
	ImGui::ColorEdit4("Particle end color", glm::value_ptr(m_ParticleProperties.ColorEnd));
	ImGui::SliderFloat("Particle lifetime", &m_ParticleProperties.Lifetime, 0.0f, 100.0f);
	ImGui::Checkbox("Square Particles", &m_SquareParticles);
	if (m_SquareParticles)
		ImGui::Checkbox("Rotate", &m_ParticleProperties.DoesRotate);

	ImGui::End();
}
