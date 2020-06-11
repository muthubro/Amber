#include "abpch.h"
#include "Amber/Core/Time.h"

#include <GLFW/glfw3.h>

namespace Amber
{

double Time::TimeSinceInit()
{
    return glfwGetTime();
}

}