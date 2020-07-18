#pragma once

#include "Amber/Renderer/RendererAPI.h"

namespace Amber 
{

class OpenGLRendererAPI : public RendererAPI 
{
public:
    void Init() override;

    void SetViewport(int x, int y, uint32_t width, uint32_t height) override;

    void SetClearColor(const glm::vec4& color) override;
    void Clear() override;

    void SetLineThickness(float thickness) override;
    void SetPointSize(float size) override;
    void SetRasterizationMode(RasterizationMode mode) override;
    void SetStencilFunction(ComparisonFunc func, uint8_t ref, uint8_t mask) override;
    void SetStencilMask(uint8_t mask) override;
    void SetStencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation depthPass) override;

    void DrawIndexed(uint32_t indexCount, PrimitiveType type = PrimitiveType::Triangles, bool depthTest = true, bool stencilTest = false) override;
    void DrawIndexedOffset(uint32_t indexCount, PrimitiveType type = PrimitiveType::Triangles, void* indexBufferPointer = 0, uint32_t offset = 0, bool depthTest = true, bool stencilTest = false) override;
};

}
