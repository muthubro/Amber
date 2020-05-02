#include "MyGameLayer.h"

#include <ImGui/imgui.h>

static const char* game =
"                                                      "
"                                                      "
"                                                      "
"                                                      "
"                                                      "
"                                                      "
"                                                      "
"                           5                          "
"                                                      "
"                               3224                   "
"                                   34                 "
"                        324                           "
"                                                      "
"                 222                     22222        "
"    P          22111                     1111122      "
"222222222222222111112222222222222222222221111111222222"
"111111111111111111111111111111111111111111111111111111"
"111111111111111111111111111111111111111111111111111111"
"111111111111111111111111111111111111111111111111111111"
"111111111111111111111111111111111111111111111111111111"
"111111111111111111111111111111111111111111111111111111"
"111111111111111111111111111111111111111111111111111111"
"111111111111111111111111111111111111111111111111111111"
"111111111111111111111111111111111111111111111111111111";

static const uint32_t levelWidth = 54;
static const uint32_t levelHeight = 24;


MyGameLayer::MyGameLayer()
	: Layer("My Game"), m_Level(game, levelWidth, levelHeight),
		m_Viewport(0.0f, (float)Application::Get().GetWindow().GetWidth(), 0.0f, (float)Application::Get().GetWindow().GetHeight())
{
}

void MyGameLayer::OnAttach()
{
	auto& playerPos = m_Level.GetPlayerPosition();
	float tileSize = m_Level.GetTileSize();
	m_Viewport.SetPosition(playerPos.x, playerPos.y, (levelWidth - 1) * tileSize, (levelHeight - 1) * tileSize);

	RenderCommand::EnableBlending();
}

void MyGameLayer::OnDetach() {}

void MyGameLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(MyGameLayer::OnWindowResize));

	m_Level.OnEvent(e);
}

void MyGameLayer::OnUpdate(Timestep ts)
{
	// Update
	m_Level.OnUpdate(ts);

	auto& playerPos = m_Level.GetPlayerPosition();
	float tileSize = m_Level.GetTileSize();
	m_Viewport.SetPosition(playerPos.x, playerPos.y, (levelWidth - 1) * tileSize, (levelHeight - 1) * tileSize);

	// Render
	RenderCommand::SetClearColor(Color::SkyBlue);
	RenderCommand::Clear();

	Renderer2D::ResetStats();

	auto& bounds = m_Viewport.GetBounds();

	Renderer2D::BeginScene(*m_Viewport.GetCamera());
	m_Level.OnRender(bounds.Left, bounds.Right, bounds.Bottom, bounds.Top);
	Renderer2D::EndScene();
}

bool MyGameLayer::OnWindowResize(WindowResizeEvent& e)
{
	CreateCamera(e.GetWidth(), e.GetHeight());
	return false;
}

void MyGameLayer::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::End();
}

void MyGameLayer::CreateCamera(uint32_t width, uint32_t height)
{
	auto& playerPos = m_Level.GetPlayerPosition();

	float left = playerPos.x - width * 0.5f;
	float right = playerPos.x + width * 0.5f;
	float bottom = playerPos.y - height * 0.5f;
	float top = playerPos.y + height * 0.5f;

	m_Viewport.SetBounds({ left, right, bottom , top });
}
