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

    void DrawIndexed(uint32_t indexCount, PrimitiveType type = PrimitiveType::Triangles, bool depthTest = true) override;
};

}
