#pragma once

#include <vector>

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
	struct ProfileResult
	{
		const char* Name;
		float Time;

		ProfileResult(const char* name, float time)
			: Name(name), Time(time) {}
	};
	std::vector<ProfileResult> m_ProfileResults;

	Hazel::OrthographicCameraController m_CameraController;
	Hazel::Ref<Hazel::Texture2D> m_CheckerboardTexture;
};
