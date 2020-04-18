#include "hzpch.h"
#include "Application.h"

#include <glad/glad.h>

#include "Input.h"
#include "Window.h"

#include "Renderer/Renderer.h"

namespace Hazel 
{

Application* Application::s_Instance = nullptr;

Application::Application()
	: m_Camera(-1.6f, 1.6f, -0.9, 0.9f)
{
	HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);

	// Draw square
	std::string squareVertexSource = R"(
		#version 410 core

		layout(location = 0) in vec3 a_Position;

		uniform mat4 u_ViewProjection;

		void main() {
			gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
		}
	)";

	std::string squareFragmentSource = R"(
		#version 410 core

		layout(location = 0) out vec4 color;

		void main() {
			color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}
	)";

	m_SquareShader.reset(new Shader(squareVertexSource, squareFragmentSource));

	m_SquareVA.reset(VertexArray::Create());

	float squareVertices[] = {
		-0.75f, -0.75f, 0.0f,
		 0.75f, -0.75f, 0.0f,
		 0.75f,  0.75f, 0.0f,
		-0.75f,  0.75f, 0.0f
	};
	std::shared_ptr<VertexBuffer> squareVB;
	squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	squareVB->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
	});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[] = { 0, 1, 2, 2, 3, 0 };
	std::shared_ptr<IndexBuffer> squareIB;
	squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

	m_SquareVA->SetIndexBuffer(squareIB);

	// Draw triangle
	std::string triangleVertexSource = R"(
		#version 410 core

		layout(location = 0) in vec3 a_Position;
		layout(location = 1) in vec4 a_Color;

		layout(location = 0) out vec4 v_Color;

		uniform mat4 u_ViewProjection;

		void main() {
			v_Color = a_Color;
			gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
		}
	)";

	std::string triangleFragmentSource = R"(
		#version 410 core

		layout(location = 0) in vec4 v_Color;

		layout(location = 0) out vec4 color;

		void main() {
			color = v_Color;
		}
	)";

	m_TriangleShader.reset(new Shader(triangleVertexSource, triangleFragmentSource));

	m_TriangleVA.reset(VertexArray::Create());

	float triangleVertices[] = {
		-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
		 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
		 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	};
	std::shared_ptr<VertexBuffer> triangleVB;
	triangleVB.reset(VertexBuffer::Create(triangleVertices, sizeof(triangleVertices)));
	triangleVB->SetLayout({
		{ ShaderDataType::Float3, "a_Position" },
		{ ShaderDataType::Float4, "a_Color" }
	});
	m_TriangleVA->AddVertexBuffer(triangleVB);

	uint32_t triangleIndices[] = { 0, 1, 2 };
	std::shared_ptr<IndexBuffer> triangleIB;
	triangleIB.reset(IndexBuffer::Create(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t)));

	m_TriangleVA->SetIndexBuffer(triangleIB);
}

Application::~Application() {}

void Application::Run() 
{
	while (m_Running) 
	{
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		RenderCommand::Clear();

		m_Camera.SetPosition({ 0.5f, 0.5f, 0.0f });
		m_Camera.SetRotation(45.0f);

		Renderer::BeginScene(m_Camera);

		Renderer::Submit(m_SquareShader, m_SquareVA);
		Renderer::Submit(m_TriangleShader, m_TriangleVA);

		Renderer::EndScene();

		for (Layer* layer : m_LayerStack)
			layer->OnUpdate();

		m_ImGuiLayer->Begin();
		for (Layer* layer : m_LayerStack)
			layer->OnImGuiRender();
		m_ImGuiLayer->End();

		m_Window->OnUpdate();
	}
}

void Application::OnEvent(Event& event) 
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) 
	{
		(*--it)->OnEvent(event);
		if (event.Handled)
			break;
	}
}

void Application::PushLayer(Layer* layer) 
{
	m_LayerStack.PushLayer(layer);
}

void Application::PushOverlay(Layer* overlay) 
{
	m_LayerStack.PushOverlay(overlay);
}

bool Application::OnWindowClose(WindowCloseEvent& event) 
{
	m_Running = false;
	return true;
}

}
