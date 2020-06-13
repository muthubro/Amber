#include <Amber.h>
#include <Amber/EntryPoint.h>

#include "EditorLayer.h"

namespace Amber
{
    
class EditorApplication : public Application
{
public:
    EditorApplication(const ApplicationProps& props)
        : Application(props) 
    {
        PushLayer(new EditorLayer());
    }
};

Application* CreateApplication()
{
    return new EditorApplication({ "Amber Editor", 1600, 900 });
}

}
