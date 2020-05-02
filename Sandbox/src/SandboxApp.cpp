#include <Amber.h>
#include <Amber/Core/EntryPoint.h>

#include "ExampleLayer.h"
#include "Sandbox2D.h"
#include "MyGame/MyGameLayer.h"

class Sandbox : public Amber::Application 
{
public:
	Sandbox() 
	{
		//PushLayer(new ExampleLayer());
		//PushLayer(new Sandbox2D());
		PushLayer(new MyGameLayer());
	}

	~Sandbox() {}
};

Amber::Application* Amber::CreateApplication() 
{
	return new Sandbox();
}