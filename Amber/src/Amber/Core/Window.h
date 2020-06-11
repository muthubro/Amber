#pragma once

#include <functional>
#include <string>

#include "Amber/Core/Base.h"
#include "Amber/Core/Events/Event.h"

namespace Amber 
{

struct WindowProps 
{
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    WindowProps(const std::string& title="Amber Engine",
                unsigned int width=1280,
                unsigned int height=720)
        : Title(title), Width(width), Height(height) {}
};

class Window : public RefCounted
{
public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Window() = default;

    virtual void OnUpdate() = 0;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual void SetEventCallback(const EventCallbackFn& fn) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;

    virtual void* GetNativeWindow() const = 0;

    static Scope<Window> Create(const WindowProps& props = WindowProps());
};

}
