#pragma once

#include <Hazel.h>

#include "Color.h"
#include "Level.h"
#include "Viewport.h"

using namespace Hazel;

class MyGameLayer : public Layer
{
public:
	MyGameLayer();
	~MyGameLayer() = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnEvent(Event& e) override;
	void OnUpdate(Timestep ts) override;
	void OnImGuiRender() override;

	bool OnWindowResize(WindowResizeEvent& e);

private:
	Level m_Level;
	Viewport m_Viewport;

	
	void CreateCamera(uint32_t width, uint32_t height);
};
