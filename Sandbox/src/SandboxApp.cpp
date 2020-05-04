#include <Amber.h>
#include <Amber/Core/EntryPoint.h>

#include "ExampleLayer.h"
#include "Sandbox2D.h"

class Sandbox : public Amber::Application 
{
public:
	Sandbox() 
	{
		PushLayer(new Sandbox2D());
	}

	~Sandbox() {}
};

Amber::Application* Amber::CreateApplication() 
{
	return new Sandbox();
}