#pragma once

#include "Amber/Renderer/RendererAPI.h"

namespace Amber 
{

class OpenGLRendererAPI : public RendererAPI 
{
public:
    void Init() override;

    void SetViewport(int x, int y, uint32_t width, uint32_t height) override;

    virtual void EnableBlending() override;
    virtual void DisableBlending() override;

    virtual void EnableDepthBuffer() override;
    virtual void DisableDepthBuffer() override;

    void SetClearColor(const glm::vec4& color) override;
    void Clear() override;

    void DrawIndexed(uint32_t indexCount) override;
};

}
