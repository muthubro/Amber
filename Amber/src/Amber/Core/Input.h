#pragma once

#include "Amber/Core/Base.h"
#include "Amber/Core/KeyCodes.h"
#include "Amber/Core/MouseCodes.h"

namespace Amber 
{

class Input 
{
public:
    static bool IsKeyPressed(KeyCode keycode);
    
    static bool IsMouseButtonPressed(MouseCode button);
    static std::pair<float, float> GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();
};

}