#pragma once

#include <Amber.h>

class ExampleLayer : public Amber::Layer
{
public:
    ExampleLayer();

    void OnUpdate(Amber::Timestep ts) override;
    void OnEvent(Amber::Event& e) override;
    void OnImGuiRender() override;

private:
    Amber::ShaderLibrary m_ShaderLibrary;
    Amber::Ref<Amber::Shader> m_FlatColorShader;
    Amber::Ref<Amber::Shader> m_TriangleShader;

    Amber::Ref<Amber::Texture2D> m_Texture, m_ChernoLogoTexture;

    Amber::Ref<Amber::VertexArray> m_SquareVA;
    Amber::Ref<Amber::VertexArray> m_TriangleVA;

    Amber::OrthographicCameraController m_CameraController;

    float m_CameraTranslationSpeed;
    float m_CameraRotationSpeed;

    int m_SquareRows, m_SquareColumns;
    glm::vec3 m_SquareColor1, m_SquareColor2;

    glm::vec3 m_SquarePosition;
    float m_SquareMoveSpeed;
};
