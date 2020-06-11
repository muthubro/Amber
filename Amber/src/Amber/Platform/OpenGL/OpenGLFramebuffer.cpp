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
    m_ColorAttachments.reserve(m_Specification.ColorAttachmentCount);
    Reset();
    FramebufferPool::GetGlobal()->Add(WeakRef<Framebuffer>(this));
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    RenderCommand::Submit([=]() {
        AB_PROFILE_FUNCTION();

        glDeleteFramebuffers(1, &m_RendererID);
        for (auto colorAttachment : m_ColorAttachments)
            colorAttachment.Reset();
        
        if (m_Specification.DepthAttachmentType == DepthBufferType::Texture)
            glDeleteTextures(1, &m_DepthAttachment);
        else
            glDeleteRenderbuffers(1, &m_DepthAttachment);
    });
}

void OpenGLFramebuffer::Reset()
{
    // TODO: Delete depth buffer too
    if (m_RendererID)
        m_ColorAttachments.clear();

    for (uint32_t i = 0; i < m_Specification.ColorAttachmentCount; i++)
    {
        m_ColorAttachments.push_back(Texture2D::Create(
            FramebufferFormatToTextureFormat(m_Specification.Format),
            m_Specification.Width, m_Specification.Height,
            TextureWrap::Clamp, TextureFilter::Linear,
            m_Specification.Samples));

        m_ColorAttachments[i]->Bind();
    }

    // Renderbuffers cannot be used if we set fixedSampleLocations to false for MS texture
    if (m_Specification.Samples > 1)
        m_Specification.DepthAttachmentType = DepthBufferType::Texture;

    Ref<OpenGLFramebuffer> instance = this;
    RenderCommand::Submit([instance]() mutable {
        AB_PROFILE_FUNCTION();

        bool multisample = instance->m_Specification.Samples > 1;

        if (instance->m_Specification.DepthAttachmentType == DepthBufferType::Texture)
        {
            if (instance->m_DepthAttachment)
                glDeleteTextures(1, &instance->m_DepthAttachment);

            if (multisample)
            {
                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &instance->m_DepthAttachment);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, instance->m_DepthAttachment);
                glTextureStorage2DMultisample(
                    instance->m_DepthAttachment,
                    instance->m_Specification.Samples,
                    instance->m_Specification.StencilBuffer ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT32,
                    instance->m_Specification.Width, instance->m_Specification.Height, GL_FALSE);
            }
            else
            {
                glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_DepthAttachment);
                glBindTexture(GL_TEXTURE_2D, instance->m_DepthAttachment);

                glTextureParameteri(instance->m_DepthAttachment, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(instance->m_DepthAttachment, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTextureParameteri(instance->m_DepthAttachment, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(instance->m_DepthAttachment, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glTextureStorage2D(
                    instance->m_DepthAttachment, 1,
                    instance->m_Specification.StencilBuffer ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT32,
                    instance->m_Specification.Width, instance->m_Specification.Height);
            }
        }
        else if (instance->m_Specification.DepthAttachmentType == DepthBufferType::Renderbuffer)
        {
            if (instance->m_DepthAttachment)
                glDeleteRenderbuffers(1, &instance->m_DepthAttachment);
            glCreateRenderbuffers(1, &instance->m_DepthAttachment);
            glBindRenderbuffer(GL_RENDERBUFFER, instance->m_DepthAttachment);

            if (multisample)
            {
                glRenderbufferStorageMultisample(
                    GL_RENDERBUFFER,
                    instance->m_Specification.Samples,
                    instance->m_Specification.StencilBuffer ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT32,
                    instance->m_Specification.Width, instance->m_Specification.Height);
            }
            else
            {
                glRenderbufferStorage(
                    GL_RENDERBUFFER,
                    instance->m_Specification.StencilBuffer ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT32,
                    instance->m_Specification.Width, instance->m_Specification.Height);
            }
        }

        if (!instance->m_RendererID)
            glCreateFramebuffers(1, &instance->m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);

        if (multisample)
        {
            for (uint32_t i = 0; i < instance->m_Specification.ColorAttachmentCount; i++)
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, instance->m_ColorAttachments[i]->GetRendererID(), 0);

            if (instance->m_Specification.DepthAttachmentType == DepthBufferType::Texture)
            {
                glFramebufferTexture(
                    GL_FRAMEBUFFER, 
                    instance->m_Specification.StencilBuffer ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT,
                    instance->m_DepthAttachment, 0);
            }
            else if (instance->m_Specification.DepthAttachmentType == DepthBufferType::Renderbuffer)
            {
                glFramebufferRenderbuffer(
                    GL_FRAMEBUFFER,
                    instance->m_Specification.StencilBuffer ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT,
                    GL_RENDERBUFFER, instance->m_DepthAttachment);
            }
        }
        else
        {
            for (uint32_t i = 0; i < instance->m_Specification.ColorAttachmentCount; i++)
                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, instance->m_ColorAttachments[i]->GetRendererID(), 0);

            if (instance->m_Specification.DepthAttachmentType == DepthBufferType::Texture)
            {
                glFramebufferTexture(
                    GL_FRAMEBUFFER,
                    instance->m_Specification.StencilBuffer ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT,
                    instance->m_DepthAttachment, 0);
            }
            else if (instance->m_Specification.DepthAttachmentType == DepthBufferType::Renderbuffer)
            {
                glFramebufferRenderbuffer(
                    GL_FRAMEBUFFER,
                    instance->m_Specification.StencilBuffer ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT,
                    GL_RENDERBUFFER, instance->m_DepthAttachment);
            }
        }

        AB_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
}

void OpenGLFramebuffer::Bind() const
{
    Ref<const OpenGLFramebuffer> instance = this;
    RenderCommand::Submit([instance]() {
        AB_PROFILE_FUNCTION();

        glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);
    });
}

void OpenGLFramebuffer::Unbind() const
{
    RenderCommand::Submit([]() {
        AB_PROFILE_FUNCTION();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });
}

}