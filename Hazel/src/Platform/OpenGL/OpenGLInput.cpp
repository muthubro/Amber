#include "hzpch.h"
#include "OpenGLInput.h"

#include "Hazel/Application.h"

#include <GLFW/glfw3.h>

namespace Hazel {

std::unique_ptr<Input> Input::s_Instance = std::make_unique<OpenGLInput>();

bool OpenGLInput::IsKeyPressedImpl(int keycode) const {
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetKey(window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool OpenGLInput::IsMouseButtonPressedImpl(int button) const {
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetMouseButton(window, button);
	return state == GLFW_PRESS;
}

std::pair<float, float> OpenGLInput::GetMousePosImpl() const {
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return { (float)xpos, (float)ypos };
}

float OpenGLInput::GetMouseXImpl() const {
	auto [x, y] = GetMousePosImpl();
	return x;
}

float OpenGLInput::GetMouseYImpl() const {
	auto [x, y] = GetMousePosImpl();
	return y;
}

}