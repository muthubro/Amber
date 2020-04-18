#include "Hazel.h"

#include <ImGui/imgui.h>

class ExampleLayer : public Hazel::Layer 
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),
			m_CameraPosition(0.0f), m_CameraMoveSpeed(1.0f),
			m_CameraRotation(0.0f), m_CameraRotationSpeed(20.0f)
	{
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

		m_SquareShader.reset(new Hazel::Shader(squareVertexSource, squareFragmentSource));

		m_SquareVA.reset(Hazel::VertexArray::Create());

		float squareVertices[] = {
			-0.75f, -0.75f, 0.0f,
			 0.75f, -0.75f, 0.0f,
			 0.75f,  0.75f, 0.0f,
			-0.75f,  0.75f, 0.0f
		};
		std::shared_ptr<Hazel::VertexBuffer> squareVB;
		squareVB.reset(Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Hazel::ShaderDataType::Float3, "a_Position" },
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Hazel::IndexBuffer> squareIB;
		squareIB.reset(Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));

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

		m_TriangleShader.reset(new Hazel::Shader(triangleVertexSource, triangleFragmentSource));

		m_TriangleVA.reset(Hazel::VertexArray::Create());

		float triangleVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		std::shared_ptr<Hazel::VertexBuffer> triangleVB;
		triangleVB.reset(Hazel::VertexBuffer::Create(triangleVertices, sizeof(triangleVertices)));
		triangleVB->SetLayout({
			{ Hazel::ShaderDataType::Float3, "a_Position" },
			{ Hazel::ShaderDataType::Float4, "a_Color" }
		});
		m_TriangleVA->AddVertexBuffer(triangleVB);

		uint32_t triangleIndices[] = { 0, 1, 2 };
		std::shared_ptr<Hazel::IndexBuffer> triangleIB;
		triangleIB.reset(Hazel::IndexBuffer::Create(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t)));

		m_TriangleVA->SetIndexBuffer(triangleIB);
	}

	void OnUpdate(Hazel::Timestep ts) override 
	{
		if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
			m_CameraPosition.x -= m_CameraMoveSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
			m_CameraPosition.x += m_CameraMoveSpeed * ts;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_UP))
			m_CameraPosition.y += m_CameraMoveSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
			m_CameraPosition.y -= m_CameraMoveSpeed * ts;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * ts;

		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		Hazel::Renderer::BeginScene(m_Camera);

		Hazel::Renderer::Submit(m_SquareShader, m_SquareVA);
		Hazel::Renderer::Submit(m_TriangleShader, m_TriangleVA);

		Hazel::Renderer::EndScene();
	}

	void OnEvent(Hazel::Event& event) override {}

	void OnImGuiRender() override
	{
		ImGui::Begin("Example Layer Toolkit");
		ImGui::SliderFloat("Translation speed", &m_CameraMoveSpeed, 0.1f, 5.0f);
		ImGui::SliderFloat("Rotation speed", &m_CameraRotationSpeed, 10.0f, 90.0f);
		ImGui::End();
	}

private:
	std::shared_ptr<Hazel::Shader> m_SquareShader;
	std::shared_ptr<Hazel::Shader> m_TriangleShader;
	std::shared_ptr<Hazel::VertexArray> m_SquareVA;
	std::shared_ptr<Hazel::VertexArray> m_TriangleVA;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed;

	float m_CameraRotation;
	float m_CameraRotationSpeed;

	Hazel::OrthographicCamera m_Camera;
};

class Sandbox : public Hazel::Application 
{
public:
	Sandbox() 
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};

Hazel::Application* Hazel::CreateApplication() 
{
	return new Sandbox();
}