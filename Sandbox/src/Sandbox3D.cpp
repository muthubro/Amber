#include "Sandbox3D.h"

#include <glm/glm.hpp>
#include <ImGui/imgui.h>

Sandbox3D::Sandbox3D()
{
}

void Sandbox3D::OnAttach()
{
    using namespace Amber;

    auto environment = Environment::Load("assets/env/birchwood_4k.hdr");

    m_Scene = Ref<Scene>::Create("Model Scene");
    m_Scene->SetCamera(Camera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f)));
    m_Scene->SetEnvironment(environment);

    m_MeshEntity = m_Scene->CreateEntity("Test entity");

    auto mesh = Ref<Mesh>::Create("assets/meshes/TestScene.fbx");
    m_MeshEntity->SetMesh(mesh);

    m_MeshMaterial = mesh->GetMaterial();

    auto& light = m_Scene->GetLight();
    light.Direction = { -0.5f, 0.5f, 1.0f };
    light.Radiance = { 1.0f, 1.0f, 1.0f };
}

void Sandbox3D::OnEvent(Amber::Event& e)
{
    m_Scene->GetCamera().OnEvent(e);
}

void Sandbox3D::OnUpdate(Amber::Timestep ts)
{
    Amber::RenderCommand::SetClearColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    Amber::RenderCommand::Clear();

    //Amber::Renderer::SubmitMesh("assets/models/backpack/backpack.obj", m_Camera.GetViewProjection());
    m_Scene->OnUpdate(ts);

    m_Scene->GetCamera().OnUpdate(ts);
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
