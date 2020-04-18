#pragma once

#include "Hazel/Input.h"

namespace Hazel 
{

class OpenGLInput : public Input 
{
protected:
	inline bool IsKeyPressedImpl(int keycode) const override;

	inline bool IsMouseButtonPressedImpl(int button) const override;
	inline std::pair<float, float> GetMousePosImpl() const override;
	inline float GetMouseXImpl() const override;
	inline float GetMouseYImpl() const override;
};

}
