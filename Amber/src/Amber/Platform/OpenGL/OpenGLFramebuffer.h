#pragma once

#include "Amber/Renderer/Framebuffer.h"

namespace Amber
{

class OpenGLFramebuffer : public Framebuffer
{
public:
    OpenGLFramebuffer(const FramebufferSpecification& spec);
    ~OpenGLFramebuffer();

    void Reset() override;
    void Resize(uint32_t width, uint32_t height) override;

    void Bind() const override;
    void Unbind() const override;
    
    RendererID GetRendererID() const override { return m_RendererID; }
    const std::vector<Ref<Texture2D>>& GetColorAttachments() const override { return m_ColorAttachments; }
    RendererID GetDepthAttachment() const override { return m_DepthAttachment; }
    
    FramebufferSpecification& GetSpecification() override { return m_Specification; }
    const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

private:
    RendererID m_RendererID = 0;
    std::vector<Ref<Texture2D>> m_ColorAttachments;
    // TODO: Figure out a way to store the actual depth attachment object (texture / renderbuffer)
    RendererID m_DepthAttachment = 0;
    FramebufferSpecification m_Specification;
};

}
