#pragma once

#include "Amber/Core/Core.h"

#ifdef AB_PLATFORM_WINDOWS

extern Amber::Application* Amber::CreateApplication();

int main(int argc, char** argv) 
{
    AB_LOG_INIT();
    AB_CORE_WARN("Logging initialized.");

    AB_PROFILE_BEGIN_SESSION("Startup", "AmberProfile-Startup.json");
    auto app = Amber::CreateApplication();
    AB_PROFILE_END_SESSION();

    AB_PROFILE_BEGIN_SESSION("Runtime", "AmberProfile-Runtime.json");
    app->Run();
    AB_PROFILE_END_SESSION();
    
    AB_PROFILE_BEGIN_SESSION("Shutdown", "AmberProfile-Shutdown.json");
    delete app;
    AB_PROFILE_END_SESSION();
}

#endif // AB_PLATFORM_WINDOWS
