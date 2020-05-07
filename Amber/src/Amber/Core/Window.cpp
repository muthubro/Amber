#include "abpch.h"
#include "Window.h"

#ifdef AB_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Amber
{

Scope<Window> Window::Create(const WindowProps& props)
{
#ifdef AB_PLATFORM_WINDOWS
    return CreateScope<WindowsWindow>(props);
#else
    AB_CORE_ASSERT(false, "Unknown platform!");
    return nullptr;
#endif
}

}