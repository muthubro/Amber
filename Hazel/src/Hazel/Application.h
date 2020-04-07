#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Hazel {

class HAZEL_API Application
{
public:
	Application();
	virtual ~Application();

	void Run();
};

// For use by CLIENT only
Application* CreateApplication();

}
