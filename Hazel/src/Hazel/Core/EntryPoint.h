#pragma once

#include "Hazel/Core/Core.h"

#ifdef HZ_PLATFORM_WINDOWS

int main() 
{
	HZ_LOG_INIT();
	HZ_CORE_WARN("Logging initialized.");

	auto app = Hazel::CreateApplication();
	app->Run();
	delete app;
}

#endif // HZ_PLATFORM_WINDOWS
