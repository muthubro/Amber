#pragma once

#include "Amber/Renderer/RenderCommandQueue.h"
#include "Amber/Renderer/RendererAPI.h"

namespace Amber 
{

class RenderCommand 
{
public:
    static void Init() { Submit([=]() { s_RendererAPI->Init(); }); }

    static void SetViewPort(int x, int y, uint32_t width, uint32_t height) { Submit([=]() { s_RendererAPI->SetViewport(x, y, width, height); }); }

    static void SetClearColor(const glm::vec4& color) { Submit([=]() { s_RendererAPI->SetClearColor(color); }); }
    static void Clear() { Submit([=]() { s_RendererAPI->Clear(); }); }

    static void SetLineThickness(float thickness) { Submit([=]() { s_RendererAPI->SetLineThickness(thickness); }); }
    static void SetPointSize(float size) { Submit([=]() { s_RendererAPI->SetPointSize(size); }); }
    static void SetRasterizationMode(RasterizationMode mode) { Submit([=]() { s_RendererAPI->SetRasterizationMode(mode); }); }
    static void SetStencilFunction(ComparisonFunc func, uint8_t ref, uint8_t mask) 
    { 
        Submit([=]() { s_RendererAPI->SetStencilFunction(func, ref, mask); }); 
    }

    static void SetStencilMask(uint8_t mask) { Submit([=]() { s_RendererAPI->SetStencilMask(mask); }); }

    static void SetStencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation depthPass)
    { 
        Submit([=]() { s_RendererAPI->SetStencilOperation(stencilFail, depthFail, depthPass); });
    }

    static void DrawIndexed(uint32_t indexCount, PrimitiveType type, bool depthTest = true, bool stencilTest = false)
    { 
        Submit([=]() { s_RendererAPI->DrawIndexed(indexCount, type, depthTest, stencilTest); }); 
    }

    static void DrawIndexedOffset(uint32_t indexCount, PrimitiveType type, void* indexBufferPointer, uint32_t offset, bool depthTest = true, bool stencilTest = false) 
    {
        Submit([=]() { s_RendererAPI->DrawIndexedOffset(indexCount, type, indexBufferPointer, offset, depthTest, stencilTest); });
    }

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
