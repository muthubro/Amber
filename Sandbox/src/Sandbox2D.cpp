#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>

Sandbox2D::Sandbox2D()
    : Layer("Sandbox 2D") {}

void Sandbox2D::OnAttach()
{
    AB_PROFILE_FUNCTION();

    m_CheckerboardTexture = Amber::Texture2D::Create("assets/textures/Checkerboard.png", true, true, Amber::TextureWrap::Repeat, Amber::TextureFilter::Nearest);
    m_Camera->SetProjectionType(Amber::SceneCamera::ProjectionType::Orthographic);
    
    Amber::RenderCommand::SetViewPort(0, 0, Amber::Application::Get().GetWindow().GetWidth(), Amber::Application::Get().GetWindow().GetHeight());
}

void Sandbox2D::OnDetach() 
{
    AB_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Amber::Timestep ts)
{
    AB_PROFILE_FUNCTION();

    // Render
    {
        AB_PROFILE_SCOPE("Renderer Prep");
        Amber::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Amber::RenderCommand::Clear();
    }

    {
        AB_PROFILE_SCOPE("Renderer Draw");

        Amber::Renderer2D::ResetStats();

        static float rotation = 0.0f;
        rotation += ts * 50.0f;

        Amber::Renderer2D::BeginScene(m_Camera->GetProjectionMatrix());

        Amber::Renderer2D::DrawQuad({ { 0.5f, -0.5f }, { 0.5f, 0.75f }, glm::radians(30.0f), { 0.2f, 0.3f, 0.8f, 1.0f } });

        Amber::Renderer2D::QuadData quadData;
        quadData.SetTransform({ -1.0f, 0.0f, 0.0f }, { 0.8f, 0.8f }, 0.0f);
        quadData.Color = {
            { 0.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f, 1.0f },
            { 1.0f, 1.0f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f, 1.0f },
        };
        Amber::Renderer2D::DrawQuad(quadData);

        quadData.SetTransform({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, 0.0f);
        quadData.SetColor(glm::vec4(1.0f));
        quadData.Texture = m_CheckerboardTexture;
        quadData.TilingFactor = 10.0f;
        Amber::Renderer2D::DrawQuad(quadData);

        quadData.SetTransform({ -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, glm::radians(rotation));
        quadData.Texture = m_CheckerboardTexture;
        quadData.TilingFactor = 20.0f;
        quadData.Color = {
            { 1.0f, 0.6f, 0.3f, 1.0f },
            { 0.6f, 0.3f, 0.0f, 1.0f },
            { 0.3f, 0.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.6f, 1.0f },
        };
        Amber::Renderer2D::DrawQuad(quadData);

        Amber::Renderer2D::DrawLine({ 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f });

        Amber::Renderer2D::EndScene();
        Amber::Renderer::WaitAndRender();

        Amber::Renderer2D::BeginScene(m_Camera->GetProjectionMatrix());
        for (float y = -5.0f; y < 5.0f; y += 0.5f)
        {
            for (float x = -5.0f; x < 5.0f; x += 0.5f)
            {
                glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
                Amber::Renderer2D::DrawQuad({ { x, y }, { 0.45f, 0.45f }, 0.0f, color });
            }
        }
        Amber::Renderer2D::EndScene();
    }
}

void Sandbox2D::OnEvent(Amber::Event& e)
{
}

void Sandbox2D::OnImGuiRender()
{
    AB_PROFILE_FUNCTION();

    ImGui::Begin("Settings");

    auto stats = Amber::Renderer2D::GetStats();
    ImGui::Text("Renderer2D Stats:");
    ImGui::Text("Draw Calls: %d", stats.DrawCalls);
    ImGui::Text("Quads: %d", stats.QuadCount);
    ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
    ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

    ImGui::End();
}
