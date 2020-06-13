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
    // TODO: Do this with depth buffer too

    std::vector<Ref<Texture2D>> newColorAttachments;
    newColorAttachments.reserve(m_Specification.ColorAttachmentCount);
    for (uint32_t i = 0; i < m_Specification.ColorAttachmentCount; i++)
    {
        newColorAttachments.push_back(Texture2D::Create(
            FramebufferFormatToTextureFormat(m_Specification.Format),
            m_Specification.Width, m_Specification.Height,
            TextureWrap::Clamp, TextureFilter::Linear,
            m_Specification.Samples));

        newColorAttachments[i]->Bind();
    }

    // Renderbuffers cannot be used if we set fixedSampleLocations to false for MS texture
    if (m_Specification.Samples > 1)
        m_Specification.DepthAttachmentType = DepthBufferType::Texture;

    uint32_t width = m_Specification.Width;
    uint32_t height = m_Specification.Height;
    Ref<OpenGLFramebuffer> instance = this;
    RenderCommand::Submit([instance, width, height, newColorAttachments]() mutable {
        AB_PROFILE_FUNCTION();

        instance->m_ColorAttachments.swap(newColorAttachments);

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
                    width, height, GL_FALSE);
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
                    width, height);
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
                    width, height);
            }
            else
            {
                glRenderbufferStorage(
                    GL_RENDERBUFFER,
                    instance->m_Specification.StencilBuffer ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT32,
                    width, height);
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

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
{
    if (m_Specification.Width == width && m_Specification.Height == height)
        return;

    m_Specification.Width = width;
    m_Specification.Height = height;
    Reset();
}

void OpenGLFramebuffer::Bind() const
{
    Ref<const OpenGLFramebuffer> instance = this;
    RenderCommand::Submit([instance]() {
        AB_PROFILE_FUNCTION();

        glBindFramebuffer(GL_FRAMEBUFFER, instance->m_RendererID);
        glViewport(0, 0, instance->m_Specification.Width, instance->m_Specification.Height);
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