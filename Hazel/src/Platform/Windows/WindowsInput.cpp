#include "hzpch.h"
#include "WindowsInput.h"

#include "Hazel/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Hazel
{

bool WindowsInput::IsKeyPressedImpl(KeyCode keycode)
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetKey(window, static_cast<int32_t>(keycode));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::IsMouseButtonPressedImpl(MouseCode button)
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
	return state == GLFW_PRESS;
}

std::pair<float, float> WindowsInput::GetMousePositionImpl()
{
	auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	xpos = xpos / Application::Get().GetWindow().GetWidth() - 0.5;
	ypos = 0.5 - ypos / Application::Get().GetWindow().GetHeight();
	return { (float)xpos * 2.0f, (float)ypos * 2.0f };
}

float WindowsInput::GetMouseXImpl()
{
	auto [x, y] = GetMousePositionImpl();
	return x;
}

float WindowsInput::GetMouseYImpl()
{
	auto [x, y] = GetMousePositionImpl();
	return y;
}

}