#pragma once

#include "Hazel/Core/Input.h"

namespace Hazel
{

class WindowsInput : public Input
{
protected:
	bool IsKeyPressedImpl(KeyCode keycode) const override;

	bool IsMouseButtonPressedImpl(MouseCode button) const override;
	std::pair<float, float> GetMousePositionImpl() const override;
	float GetMouseXImpl() const override;
	float GetMouseYImpl() const override;
};

}