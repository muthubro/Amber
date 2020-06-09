#pragma once

#include "Amber/Core/Base.h"

#include "Amber/Renderer/Framebuffer.h"

namespace Amber
{

struct RenderPassSpecification
{
    Ref<Framebuffer> TargetFramebuffer;
};

class RenderPass : public RefCounted
{
public:
    virtual ~RenderPass() {}

    virtual RenderPassSpecification& GetSpecification() = 0;
    virtual const RenderPassSpecification& GetSpecification() const = 0;

    static Ref<RenderPass> Create(const RenderPassSpecification& spec);
};

}
