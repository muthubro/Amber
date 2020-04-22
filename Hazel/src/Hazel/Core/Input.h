#pragma once

#include <memory>

#include "Hazel/Core/Core.h"
#include "Hazel/Core/KeyCodes.h"
#include "Hazel/Core/MouseCodes.h"

namespace Hazel 
{

class Input 
{
public:
	virtual ~Input() = default;

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	static bool IsKeyPressed(KeyCode keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
	
	static bool IsMouseButtonPressed(MouseCode button) { return s_Instance->IsMouseButtonPressedImpl(button); }
	static std::pair<float, float> GetMousePosition() { return s_Instance->GetMousePositionImpl(); }
	static float GetMouseX() { return s_Instance->GetMouseXImpl(); };
	static float GetMouseY() { return s_Instance->GetMouseYImpl(); }

protected:
	Input() = default;

	virtual	bool IsKeyPressedImpl(KeyCode keycode) const = 0;

	virtual	bool IsMouseButtonPressedImpl(MouseCode button) const = 0;
	virtual	std::pair<float, float> GetMousePositionImpl() const = 0;
	virtual	float GetMouseXImpl() const = 0;
	virtual	float GetMouseYImpl() const = 0;

private:
	static Scope<Input> s_Instance;
};

}