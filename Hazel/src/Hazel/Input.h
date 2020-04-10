#pragma once

#include "hzpch.h"

#include "Core.h"

namespace Hazel {

class HAZEL_API Input {
public:
	static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
	
	static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
	static std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); }
	static float GetMouseX() { return s_Instance->GetMouseXImpl(); };
	static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

protected:
	virtual	bool IsKeyPressedImpl(int keycode) const = 0;

	virtual	bool IsMouseButtonPressedImpl(int button) const = 0;
	virtual	std::pair<float, float> GetMousePosImpl() const = 0;
	virtual	float GetMouseXImpl() const = 0;
	virtual	float GetMouseYImpl() const = 0;

private:
	static Input* s_Instance;
};

}