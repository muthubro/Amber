#pragma once

#include "Hazel/Core/Core.h"

#ifdef HZ_PLATFORM_WINDOWS

int main(int argc, char** argv) 
{
	HZ_LOG_INIT();
	HZ_CORE_WARN("Logging initialized.");

	HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile-Startup.json");
	auto app = Hazel::CreateApplication();
	HZ_PROFILE_END_SESSION();

	HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile-Runtime.json");
	app->Run();
	HZ_PROFILE_END_SESSION();
	
	HZ_PROFILE_BEGIN_SESSION("Shutdown", "HazelProfile-Shutdown.json");
	delete app;
	HZ_PROFILE_END_SESSION();
}

#endif // HZ_PLATFORM_WINDOWS
