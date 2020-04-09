#pragma once

#include "Hazel/Window.h"

#include <GLFW/glfw3.h>

namespace Hazel {

class HAZEL_API WindowsWindow : public Window {
public:
	WindowsWindow(const WindowProps& props);
	~WindowsWindow();

	void OnUpdate() override;

	inline unsigned int GetWidth() const override { return m_Data.Width; }
	inline unsigned int GetHeight() const override { return m_Data.Height; }

	inline void SetEventCallback(const EventCallbackFn& fn) override { m_Data.EventCallback = fn; }
	void SetVSync(bool enabled) override;
	bool IsVSync() const override;

	inline void* GetNativeWindow() const override { return m_Window; }

private:
	virtual void Init(const WindowProps& props);
	virtual void Shutdown();

	struct WindowData {
		std::string Title;
		unsigned int Width, Height;
		bool VSync;

		EventCallbackFn EventCallback;
	};

	WindowData m_Data;
	GLFWwindow* m_Window;
};

}
