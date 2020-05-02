#pragma once

#include "Amber/Core/Input.h"

namespace Amber
{

class WindowsInput : public Input
{
protected:
	bool IsKeyPressedImpl(KeyCode keycode) override;

	bool IsMouseButtonPressedImpl(MouseCode button) override;
	std::pair<float, float> GetMousePositionImpl() override;
	float GetMouseXImpl() override;
	float GetMouseYImpl() override;
};

}