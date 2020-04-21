#include "Sandbox2D.h"

#include <chrono>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>

template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func)
		: m_Name(name), m_Func(func),
			m_StartTimePoint(std::chrono::high_resolution_clock::now()) {}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();

		auto startTime = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimePoint).time_since_epoch().count();
		auto endTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

		float duration = (endTime - startTime) * 0.001f;
		m_Func(duration);
	}

private:
	const char* m_Name;
	Fn m_Func;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;
	bool m_Stopped = false;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](float duration) { m_ProfileResults.emplace_back(name, duration); })

Sandbox2D::Sandbox2D()
	: Layer("Sandbox 2D"), m_CameraController(1280.0f/720.0f) {}

void Sandbox2D::OnAttach()
{
	m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach() {}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	PROFILE_SCOPE("Sandbox2D::OnUpdate");
	// Update
	{
		PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	}

	// Render
	{
		PROFILE_SCOPE("Render Prep");
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Hazel::RenderCommand::Clear();
	}

	{
		PROFILE_SCOPE("Renderer Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, 0.0f, { 0.8f, 0.3f, 0.2f, 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, 45.0f, { 0.2f, 0.3f, 0.8f, 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.0f, -0.0f, -0.1f }, { 10.0f, 10.0f }, 0.0f, m_CheckerboardTexture, { 0.6f, 0.3f, 0.6f, 0.5f });
		Hazel::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Settings");

	for (auto& result : m_ProfileResults)
	{
		char text[50];
		strcpy_s(text, "%.3fms ");
		strcat_s(text, result.Name);
		ImGui::Text(text, result.Time);
	}
	m_ProfileResults.clear();

	ImGui::End();
}
