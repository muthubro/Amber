#include "Sandbox3D.h"

#include <glm/glm.hpp>
#include <ImGui/imgui.h>

Sandbox3D::Sandbox3D()
    : m_Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
{
}

void Sandbox3D::OnEvent(Amber::Event& e)
{
    m_Camera.OnEvent(e);
}

void Sandbox3D::OnUpdate(Amber::Timestep ts)
{
    Amber::RenderCommand::Clear();

    Amber::Renderer::DrawMesh("assets/models/backpack/backpack.obj", m_Camera.GetViewProjection());

    m_Camera.OnUpdate(ts);
}

void Sandbox3D::OnImGuiRender()
{
    AB_PROFILE_FUNCTION();

    ImGui::Begin("Settings");

    auto stats = Amber::Renderer2D::GetStats();
    ImGui::Text("Renderer Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();
}
