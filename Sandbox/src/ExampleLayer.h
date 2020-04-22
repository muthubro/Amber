#pragma once

#include <Hazel.h>

class ExampleLayer : public Hazel::Layer
{
public:
	ExampleLayer();

	void OnUpdate(Hazel::Timestep ts) override;
	void OnEvent(Hazel::Event& e) override;
	void OnImGuiRender() override;

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
