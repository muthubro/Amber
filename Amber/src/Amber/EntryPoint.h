#pragma once

#include "Amber/Core/Base.h"

#ifdef AB_PLATFORM_WINDOWS

extern Amber::Application* Amber::CreateApplication();

int main(int argc, char** argv) 
{
    AB_LOG_INIT();
    AB_CORE_TRACE("Amber Engine {}", AMBER_BUILD_ID);

    AB_PROFILE_BEGIN_SESSION("Startup", "AmberProfile-Startup.json");
    auto app = Amber::CreateApplication();
    AB_PROFILE_END_SESSION();

    AB_PROFILE_BEGIN_SESSION("Runtime", "AmberProfile-Runtime.json");
    app->Run();
    AB_PROFILE_END_SESSION();
    
    AB_PROFILE_BEGIN_SESSION("Shutdown", "AmberProfile-Shutdown.json");
    delete app;
    AB_PROFILE_END_SESSION();

    AB_CORE_WARN("Shutting down...");
}

#endif // AB_PLATFORM_WINDOWS
