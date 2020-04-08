#pragma once

#include "Core.h"
#include "Window.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace Hazel {

class HAZEL_API Application
{
public:
	Application();
	virtual ~Application();

	void Run();
	void OnEvent(Event&);

	bool OnWindowClose(WindowCloseEvent&);

private:
	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
};

// For use by CLIENT only
Application* CreateApplication();

}
