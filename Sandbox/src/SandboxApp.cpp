#include <Amber.h>
#include <Amber/Core/EntryPoint.h>

#include "Sandbox2D.h"
#include "Sandbox3D.h"

class Sandbox : public Amber::Application 
{
public:
    Sandbox() 
    {
        PushLayer(new Sandbox2D());
        //PushLayer(new Sandbox3D());
    }

    ~Sandbox() {}
};

Amber::Application* Amber::CreateApplication() 
{
    return new Sandbox();
}