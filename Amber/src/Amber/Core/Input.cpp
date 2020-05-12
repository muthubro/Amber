#include "abpch.h"
#include "Input.h"

#ifdef AB_PLATFORM_WINDOWS
#include "Amber/Platform/Windows/WindowsInput.h"
#endif

namespace Amber
{

Scope<Input> Input::s_Instance = Input::Create();

Scope<Input> Input::Create()
{
#ifdef AB_PLATFORM_WINDOWS
    return CreateScope<WindowsInput>();
#else
    AB_CORE_ASSERT(false, "Unknown platform!");
    return nullptr;
#endif
}

}