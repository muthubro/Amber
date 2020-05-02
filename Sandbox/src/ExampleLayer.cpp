#include "ExampleLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>

ExampleLayer::ExampleLayer()
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

	m_FlatColorShader = Amber::Shader::Create("FlatColorShader", flatColorVertexSource, flatColorFragmentSource);

	m_SquareVA = Amber::VertexArray::Create();

	float squareVertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};
	Amber::Ref<Amber::VertexBuffer> squareVB;
	squareVB = Amber::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
	squareVB->SetLayout({
		{ Amber::ShaderDataType::Float3, "a_Position" },
		{ Amber::ShaderDataType::Float2, "a_TexCoord" },
		});
	m_SquareVA->AddVertexBuffer(squareVB);

	uint32_t squareIndices[] = { 0, 1, 2, 2, 3, 0 };
	Amber::Ref<Amber::IndexBuffer> squareIB;
	squareIB = Amber::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));

	m_SquareVA->SetIndexBuffer(squareIB);

	// Draw textured square
	auto& textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

	m_Texture = Amber::Texture2D::Create("assets/textures/Checkerboard.png");
	m_ChernoLogoTexture = Amber::Texture2D::Create("assets/textures/ChernoLogo.png");

	textureShader->Bind();
	textureShader->SetInt("u_Texture", 0);

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

	m_TriangleShader = Amber::Shader::Create("PosColorShader", triangleVertexSource, triangleFragmentSource);

	m_TriangleVA = Amber::VertexArray::Create();

	float triangleVertices[] = {
		-0.2f, -0.2f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			0.2f, -0.2f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			0.0f,  0.2f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
	};
	Amber::Ref<Amber::VertexBuffer> triangleVB;
	triangleVB = Amber::VertexBuffer::Create(triangleVertices, sizeof(triangleVertices));
	triangleVB->SetLayout({
		{ Amber::ShaderDataType::Float3, "a_Position" },
		{ Amber::ShaderDataType::Float4, "a_Color" }
		});
	m_TriangleVA->AddVertexBuffer(triangleVB);

	uint32_t triangleIndices[] = { 0, 1, 2 };
	Amber::Ref<Amber::IndexBuffer> triangleIB;
	triangleIB = Amber::IndexBuffer::Create(triangleIndices, sizeof(triangleIndices) / sizeof(uint32_t));

	m_TriangleVA->SetIndexBuffer(triangleIB);
}

void ExampleLayer::OnUpdate(Amber::Timestep ts)
{
	m_CameraController.SetCameraTranslationSpeed(m_CameraTranslationSpeed);
	m_CameraController.SetCameraRotationSpeed(m_CameraRotationSpeed);
	m_CameraController.OnUpdate(ts);

	if (Amber::Input::IsKeyPressed(AB_KEY_J))
		m_SquarePosition.x -= m_SquareMoveSpeed * ts;
	else if (Amber::Input::IsKeyPressed(AB_KEY_L))
		m_SquarePosition.x += m_SquareMoveSpeed * ts;

	if (Amber::Input::IsKeyPressed(AB_KEY_I))
		m_SquarePosition.y += m_SquareMoveSpeed * ts;
	else if (Amber::Input::IsKeyPressed(AB_KEY_K))
		m_SquarePosition.y -= m_SquareMoveSpeed * ts;

	Amber::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
	Amber::RenderCommand::Clear();

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	Amber::Renderer::BeginScene(m_CameraController.GetCamera());

	for (int y = 0; y < m_SquareRows; y++)
	{
		for (int x = 0; x < m_SquareColumns; x++)
		{
			m_FlatColorShader->Bind();
			if ((x + y) % 2 == 0)
				m_FlatColorShader->SetFloat3("u_Color", m_SquareColor1);
			else
				m_FlatColorShader->SetFloat3("u_Color", m_SquareColor2);

			glm::vec3 position(
				m_SquarePosition.x + (x - m_SquareColumns / 2) * 0.11f,
				m_SquarePosition.y + (y - m_SquareRows / 2) * 0.11f,
				0.0f);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * scale;
			Amber::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
		}
	}

	auto& textureShader = m_ShaderLibrary.Get("Texture");

	m_Texture->Bind();
	Amber::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

	m_ChernoLogoTexture->Bind();
	Amber::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.0f)));

	//Amber::Renderer::Submit(m_TriangleShader, m_TriangleVA);

	Amber::Renderer::EndScene();
}

void ExampleLayer::OnEvent(Amber::Event& e)
{
	m_CameraController.OnEvent(e);
}

void ExampleLayer::OnImGuiRender()
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
