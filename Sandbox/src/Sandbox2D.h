#pragma once

#include <Hazel.h>

class Sandbox2D : public Hazel::Layer
{
public:
	Sandbox2D();
	~Sandbox2D() = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(Hazel::Timestep ts) override;
	void OnEvent(Hazel::Event& e) override;
	void OnImGuiRender() override;

private:
	Hazel::OrthographicCameraController m_CameraController;
	Hazel::Ref<Hazel::Texture2D> m_CheckerboardTexture;

	Hazel::ParticleSystem m_SquareParticleSystem;
	Hazel::ParticleSystem m_CircleParticleSystem;
	Hazel::ParticleProps m_ParticleProperties;
	bool m_SquareParticles = true;
};
