#include "abpch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

#include "Amber/Renderer/Renderer.h"

namespace Amber
{

static TextureFormat FramebufferFormatToTextureFormat(const FramebufferFormat& framebufferFormat)
{
    switch (framebufferFormat)
    {
        case FramebufferFormat::RGBA8: return TextureFormat::RGBA;
        case FramebufferFormat::RGBA16F: return TextureFormat::Float16;
    }

    AB_CORE_ASSERT(false, "Framebuffer format not recognized!");
    return TextureFormat::None;
}

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
    :m_Specification(spec)
{
    Resize(spec.Width, spec.Height, true);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    RenderCommand::Submit([this]() {
        glDeleteFramebuffers(1, &m_RendererID);
        m_ColorAttachment.Reset();
        m_DepthAttachment.Reset();
    });
}

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height, bool forceRecreate)
{
    if (!forceRecreate && (width == m_Specification.Width && height == m_Specification.Height))
        return;

    m_Specification.Width = width;
    m_Specification.Height = height;

    bool multisample = m_Specification.Samples > 1;
    if (multisample)
    {
        m_ColorAttachment = Texture2D::Create(
            FramebufferFormatToTextureFormat(m_Specification.Format),
            m_Specification.Width, m_Specification.Height,
            TextureWrap::Clamp, TextureFilter::Linear,
            m_Specification.Samples);

        m_DepthAttachment = Texture2D::Create(
            TextureFormat::DepthStencil,
            m_Specification.Width, m_Specification.Height,
            TextureWrap::Clamp, TextureFilter::Linear,
            m_Specification.Samples);
    }
    else
    {
        m_ColorAttachment = Texture2D::Create(
            FramebufferFormatToTextureFormat(m_Specification.Format),
            m_Specification.Width, m_Specification.Height);

        m_DepthAttachment = Texture2D::Create(
            TextureFormat::DepthStencil,
            m_Specification.Width, m_Specification.Height);
    }

    RenderCommand::Submit([this, multisample]() {
        if (!m_RendererID)
            glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        if (multisample)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_ColorAttachment->GetRendererID(), 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_DepthAttachment->GetRendererID(), 0);
        }
        else
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment->GetRendererID(), 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetRendererID(), 0);
        }

        AB_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
}

void OpenGLFramebuffer::Bind() const
{
    RenderCommand::Submit([this]() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    });
}

void OpenGLFramebuffer::Unbind() const
{
    RenderCommand::Submit([this]() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
}

void OpenGLFramebuffer::BindTexture(uint32_t slot) const
{
    m_ColorAttachment->Bind(slot);
}

}