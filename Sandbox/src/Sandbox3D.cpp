#include "Sandbox3D.h"

#include <glm/glm.hpp>
#include <ImGui/imgui.h>

// INSTRUCTIONS:
// Before running this, go to SceneRenderer and uncomment the commented section in CompositePass

Sandbox3D::Sandbox3D()
{
}

void Sandbox3D::OnAttach()
{
    using namespace Amber;

    auto environment = Environment::Load("assets/env/birchwood_4k.hdr");

    auto width = Application::Get().GetWindow().GetWidth();
    auto height = Application::Get().GetWindow().GetHeight();

    SceneRenderer::SetViewportSize(width, height);

    m_Scene = Ref<Scene>::Create("Model Scene");
    m_Scene->SetCamera(Camera(glm::perspectiveFov(glm::radians(45.0f), (float)width, (float)height, 0.1f, 10000.0f)));
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
    m_Scene->OnEvent(e);
}

void Sandbox3D::OnUpdate(Amber::Timestep ts)
{
    Amber::RenderCommand::SetClearColor(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
    Amber::RenderCommand::Clear();

    m_Scene->OnUpdate(ts);

    m_Scene->GetCamera().OnUpdate(ts);
}

void Sandbox3D::OnImGuiRender()
{
}
