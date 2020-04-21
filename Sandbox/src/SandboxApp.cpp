#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "Sandbox2D.h"

class ExampleLayer : public Hazel::Layer 
{
public:
	ExampleLayer()
		: Layer("Example"),
			m_CameraController(1280.0f / 720.0f, true),
			m_CameraTranslationSpeed(1.0f), m_CameraRotationSpeed(20.0f),
			m_SquareRows(11), m_SquareColumns(11),
			m_SquareColor1(0.2f, 0.3f, 0.8f),
			m_SquareColor2(0.8f, 0.3f, 0.2f),
			m_SquarePosition(0.0f), m_SquareMoveSpeed(1.0f)
	{
		// Draw square
		std::string flatColorVertexSource = R"(
			#version 410 core

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main() {
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string flatColorFragmentSource = R"(
			#version 410 core

			layout(location = 0) out vec4 color;

			uniform vec3 u_Color;

			void main() {
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader = Hazel::Shader::Create("FlatColorShader", flatColorVertexSource, flatColorFragmentSource);

		m_SquareVA = Hazel::VertexArray::Create();

		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		Hazel::Ref<Hazel::VertexBuffer> squareVB;
		squareVB = Hazel::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ Hazel::ShaderDataType::Float3, "a_Position" },
			{ Hazel::ShaderDataType::Float2, "a_TexCoord" },
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[] = { 0, 1, 2, 2, 3, 0 };
		Hazel::Ref<Hazel::IndexBuffer> squareIB;
		squareIB = Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

		m_SquareVA->SetIndexBuffer(squareIB);

		// Draw textured square
		auto& textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoLogoTexture = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");

		textureShader->Bind();
		std::dynamic_pointer_cast<Hazel::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

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

		m_TriangleShader = Hazel::Shader::Create("PosColorShader", triangleVertexSource, triangleFragmentSource);

		m_TriangleVA = Hazel::VertexArray::Create();

		float triangleVertices[] = {
			-0.2f, -0.2f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.2f, -0.2f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.2f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		Hazel::Ref<Hazel::VertexBuffer> triangleVB;
		triangleVB = Hazel::VertexBuffer::Create(triangleVertices, sizeof(triangleVertices));
		triangleVB->SetLayout({
			{ Hazel::ShaderDataType::Float3, "a_Position" },
			{ Hazel::ShaderDataType::Float4, "a_Color" }
		});
		m_TriangleVA->AddVertexBuffer(triangleVB);

		uint32_t triangleIndices[] = { 0, 1, 2 };
		Hazel::Ref<Hazel::IndexBuffer> triangleIB;
		triangleIB = Hazel::IndexBuffer::Create(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t));

		m_TriangleVA->SetIndexBuffer(triangleIB);
	}

	void OnUpdate(Hazel::Timestep ts) override 
	{
		m_CameraController.SetCameraTranslationSpeed(m_CameraTranslationSpeed);
		m_CameraController.SetCameraRotationSpeed(m_CameraRotationSpeed);
		m_CameraController.OnUpdate(ts);

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

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		Hazel::Renderer::BeginScene(m_CameraController.GetCamera());

		for (int y = 0; y < m_SquareRows; y++)
		{
			for (int x = 0; x < m_SquareColumns; x++)
			{
				m_FlatColorShader->Bind();
				if ((x + y) % 2 == 0)
					std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor1);
				else
					std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor2);

				glm::vec3 position(
					m_SquarePosition.x + (x - m_SquareColumns/2) * 0.11f, 
					m_SquarePosition.y + (y - m_SquareRows/2) * 0.11f, 
					0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * scale;
				Hazel::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto& textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

		m_ChernoLogoTexture->Bind();
		Hazel::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

		//Hazel::Renderer::Submit(m_TriangleShader, m_TriangleVA);

		Hazel::Renderer::EndScene();
	}

	void OnEvent(Hazel::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Example Layer Toolkit");

		ImGui::Text("Camera settings:");
		ImGui::SliderFloat("Camera translation speed", &m_CameraTranslationSpeed, 0.1f, 10.0f);
		ImGui::SliderFloat("Camera rotation speed", &m_CameraRotationSpeed, 10.0f, 90.0f);

		ImGui::NewLine();

		ImGui::Text("Square grid settings:");
		ImGui::SliderInt("Square rows", &m_SquareRows, 1, 20);
		ImGui::SliderInt("Square columns", &m_SquareColumns, 1, 20);
		ImGui::ColorEdit3("Square color 1", glm::value_ptr(m_SquareColor1));
		ImGui::ColorEdit3("Square color 2", glm::value_ptr(m_SquareColor2));
		ImGui::SliderFloat("Square translation speed", &m_SquareMoveSpeed, 0.1f, 10.0f);

		ImGui::End();
	}

private:
	Hazel::ShaderLibrary m_ShaderLibrary;
	Hazel::Ref<Hazel::Shader> m_FlatColorShader;
	Hazel::Ref<Hazel::Shader> m_TriangleShader;

	Hazel::Ref<Hazel::Texture2D> m_Texture, m_ChernoLogoTexture;

	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	Hazel::Ref<Hazel::VertexArray> m_TriangleVA;

	Hazel::OrthographicCameraController m_CameraController;

	float m_CameraTranslationSpeed;
	float m_CameraRotationSpeed;

	int m_SquareRows, m_SquareColumns;
	glm::vec3 m_SquareColor1, m_SquareColor2;

	glm::vec3 m_SquarePosition;
	float m_SquareMoveSpeed;
};

class Sandbox : public Hazel::Application 
{
public:
	Sandbox() 
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {}
};

Hazel::Application* Hazel::CreateApplication() 
{
	return new Sandbox();
}