#pragma once

#include "Amber/Renderer/Framebuffer.h"

namespace Amber
{

class OpenGLFramebuffer : public Framebuffer
{
public:
    OpenGLFramebuffer(const FramebufferSpecification& spec);
    ~OpenGLFramebuffer();

    void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) override;

    void Bind() const override;
    void Unbind() const override;
    void BindTexture(uint32_t slot = 0) const override;
    
    RendererID GetRendererID() const override { return m_RendererID; }
    Ref<Texture2D> GetColorAttachment() const override { return m_ColorAttachment; }
    Ref<Texture2D> GetDepthAttachment() const override { return m_DepthAttachment; }
    
    const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

private:
    RendererID m_RendererID = 0;
    Ref<Texture2D> m_ColorAttachment, m_DepthAttachment;
    FramebufferSpecification m_Specification;
};

}
