#include "Hazel.h"

#include <glm/gtc/matrix_transform.hpp>
#include <ImGui/imgui.h>

class ExampleLayer : public Hazel::Layer 
{
public:
	ExampleLayer()
		: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f),
			m_CameraPosition(0.0f), m_CameraMoveSpeed(1.0f),
			m_CameraRotation(0.0f), m_CameraRotationSpeed(20.0f),
			m_SquareRows(1), m_SquareColumns(1),
			m_SquarePosition(0.0f), m_SquareMoveSpeed(1.0f)
	{
		// Draw square
		std::string squareVertexSource = R"(
			#version 410 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main() {
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string squareFragmentSource = R"(
			#version 410 core

			layout(location = 0) out vec4 color;

			void main() {
				color = vec4(0.0f, 0.47f, 0.75f, 1.0f);
			}
		)";

		m_SquareShader.reset(new Hazel::Shader(squareVertexSource, squareFragmentSource));

		m_SquareVA.reset(Hazel::VertexArray::Create());

		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
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
			uniform mat4 u_Transform;

			void main() {
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
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

		if (Hazel::Input::IsKeyPressed(HZ_KEY_J))
			m_SquarePosition.x -= m_SquareMoveSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_L))
			m_SquarePosition.x += m_SquareMoveSpeed * ts;

		if (Hazel::Input::IsKeyPressed(HZ_KEY_I))
			m_SquarePosition.y += m_SquareMoveSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_K))
			m_SquarePosition.y -= m_SquareMoveSpeed * ts;

		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Hazel::RenderCommand::Clear();

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		Hazel::Renderer::BeginScene(m_Camera);

		for (int y = 0; y < m_SquareRows; y++)
		{
			for (int x = 0; x < m_SquareColumns; x++)
			{
				glm::vec3 position(
					m_SquarePosition.x + (x - m_SquareColumns/2) * 0.11f, 
					m_SquarePosition.y + (y - m_SquareRows/2) * 0.11f, 
					0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * scale;
				Hazel::Renderer::Submit(m_SquareShader, m_SquareVA, transform);
			}
		}
		Hazel::Renderer::Submit(m_TriangleShader, m_TriangleVA);

		Hazel::Renderer::EndScene();
	}

	void OnEvent(Hazel::Event& event) override {}

	void OnImGuiRender() override
	{
		ImGui::Begin("Example Layer Toolkit");

		ImGui::Text("Camera settings:");
		ImGui::SliderFloat("Camera translation speed", &m_CameraMoveSpeed, 0.1f, 10.0f);
		ImGui::SliderFloat("Camera rotation speed", &m_CameraRotationSpeed, 10.0f, 90.0f);

		ImGui::NewLine();

		ImGui::Text("Square grid settings:");
		ImGui::SliderInt("Square rows", &m_SquareRows, 1, 20);
		ImGui::SliderInt("Square columns", &m_SquareColumns, 1, 20);
		ImGui::SliderFloat("Square translation speed", &m_SquareMoveSpeed, 0.1f, 10.0f);

		ImGui::End();
	}

private:
	std::shared_ptr<Hazel::Shader> m_SquareShader;
	std::shared_ptr<Hazel::Shader> m_TriangleShader;
	std::shared_ptr<Hazel::VertexArray> m_SquareVA;
	std::shared_ptr<Hazel::VertexArray> m_TriangleVA;

	Hazel::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed;

	float m_CameraRotation;
	float m_CameraRotationSpeed;

	int m_SquareRows, m_SquareColumns;

	glm::vec3 m_SquarePosition;
	float m_SquareMoveSpeed;
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