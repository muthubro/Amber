#include <Hazel.h>
#include <Hazel/Core/EntryPoint.h>

#include "ExampleLayer.h"
#include "Sandbox2D.h"
#include "MyGame/MyGameLayer.h"

class Sandbox : public Hazel::Application 
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

Hazel::Application* Hazel::CreateApplication() 
{
	return new Sandbox();
}