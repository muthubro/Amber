#pragma once

#include "Amber/Renderer/RenderPass.h"

namespace Amber
{

class OpenGLRenderPass : public RenderPass
{
public:
    OpenGLRenderPass(const RenderPassSpecification& spec);
    ~OpenGLRenderPass();

    const RenderPassSpecification& GetSpecification() const override { return m_Specification; }

private:
    RenderPassSpecification m_Specification;
};

}
