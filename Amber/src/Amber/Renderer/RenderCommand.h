#pragma once

#include "Amber/Renderer/RendererAPI.h"
#include "Amber/Renderer/RenderCommandQueue.h"

namespace Amber 
{

class RenderCommand 
{
public:
    static void Init() { Submit([=]() { s_RendererAPI->Init(); }); }

    static void SetViewPort(int x, int y, uint32_t width, uint32_t height) { Submit([=]() { s_RendererAPI->SetViewport(x, y, width, height); }); }

    static void EnableBlending() { Submit([=]() { s_RendererAPI->EnableBlending(); }); }
    static void DisableBlending() { Submit([=]() { s_RendererAPI->DisableBlending(); }); }

    static void EnableDepthBuffer() { Submit([=]() { s_RendererAPI->EnableDepthBuffer(); }); }
    static void DisableDepthBuffer() { Submit([=]() { s_RendererAPI->DisableDepthBuffer(); }); }

    static void SetClearColor(const glm::vec4& color) { Submit([=]() { s_RendererAPI->SetClearColor(color); }); }
    static void Clear() { Submit([=]() { s_RendererAPI->Clear(); }); }

    //static void DrawIndexed(uint32_t indexCount) { Submit([=]() { s_RendererAPI->DrawIndexed(indexCount); }); }
    static void DrawIndexed(uint32_t indexCount);
    
    template<typename FuncT>
    static void Submit(FuncT&& func) 
    {
        auto renderCmd = [](void* ptr) {
            auto pFunc = (FuncT*)ptr;
            (*pFunc)();
            pFunc->~FuncT();
        };

        auto storageBuffer = s_CommandQueue.Allocate(renderCmd, sizeof(func));
        new (storageBuffer) FuncT(std::forward<FuncT>(func));
    }

    static RenderCommandQueue& GetCommandQueue() { return s_CommandQueue; }

private:
    static Scope<RendererAPI> s_RendererAPI;
    inline static RenderCommandQueue s_CommandQueue;
};

}
