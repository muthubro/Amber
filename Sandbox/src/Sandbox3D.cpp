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

    auto width = Application::Get().GetWindow().GetWidth();
    auto height = Application::Get().GetWindow().GetHeight();

    SceneRenderer::SetViewportSize(width, height);

    m_Scene = Ref<Scene>::Create("Model Scene");
    m_CameraEntity = m_Scene->CreateEntity("Camera");
    m_CameraEntity.AddComponent<CameraComponent>();
    m_Scene->SetEnvironment(environment);

    m_MeshEntity = m_Scene->CreateEntity("Test entity");

    auto mesh = Ref<Mesh>::Create("assets/meshes/TestScene.fbx");
    m_MeshEntity.AddComponent<MeshComponent>(mesh);

    m_MeshMaterial = mesh->GetMaterial();

    auto& light = m_Scene->GetLight();
    light.Direction = { -0.5f, 0.5f, 1.0f };
    light.Radiance = { 1.0f, 1.0f, 1.0f };

    m_FinalShader = Amber::Shader::Create("assets/shaders/SceneComposite.glsl");
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

    auto& camera = m_CameraEntity.GetComponent<Amber::CameraComponent>().Camera;

    Amber::Renderer2D::BeginScene(camera.GetProjectionMatrix() * glm::inverse(m_CameraEntity.GetComponent<Amber::TransformComponent>().Transform));

    auto material = Amber::Ref<Amber::MaterialInstance>::Create(Amber::Ref<Amber::Material>::Create(m_FinalShader));
    material->Set("u_Exposure", camera.GetExposure());
    material->Set("u_Texture", Amber::SceneRenderer::GetFinalColorBuffer());
    material->Set("u_TextureSamples", 8);
    Amber::Renderer2D::DrawFullscreenQuad(material);

    Amber::Renderer2D::EndScene();
}

void Sandbox3D::OnImGuiRender()
{
}
