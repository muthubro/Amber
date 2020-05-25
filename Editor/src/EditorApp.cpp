#include <Amber.h>
#include <Amber/EntryPoint.h>

#include "EditorLayer.h"

class EditorApplication : public Amber::Application
{
public:
    EditorApplication(const Amber::ApplicationProps& props)
        : Application(props) 
    {
        PushLayer(new EditorLayer());
    }
};

Amber::Application* Amber::CreateApplication()
{
    return new EditorApplication({ "Amber Editor", 1280, 720 });
}