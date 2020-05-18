#include "abpch.h"
#include "OpenGLTexture.h"

#include <stb_image.h>

#include "Amber/Renderer/RenderCommand.h"

namespace Amber
{

static GLenum AmberToOpenGLTextureFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::RGB:     return GL_RGB;
        case TextureFormat::RGBA:    return GL_RGBA;
        case TextureFormat::Float16: return GL_RGBA16F;
    }

    AB_CORE_ASSERT(false, "Unknown texture format!");
    return 0;
}

static GLint AmberToOpenGLInternalTextureFormat(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::RGB:     return GL_RGB8;
        case TextureFormat::RGBA:    return GL_RGBA8;
        case TextureFormat::Float16: return GL_RGBA16F;
    }

    AB_CORE_ASSERT(false, "Unknown texture format!");
    return 0;
}

static GLenum AmberToOpenGLTextureFilter(TextureFilter filter)
{
    switch (filter)
    {
        case TextureFilter::Linear:  return GL_LINEAR;
        case TextureFilter::Nearest: return GL_NEAREST;
    }

    AB_CORE_ASSERT(false, "Unknown texture filter!");
    return 0;
}

static GLenum AmberToOpenGLTextureWrap(TextureWrap wrap)
{
    switch (wrap)
    {
        case TextureWrap::Clamp:  return GL_CLAMP_TO_EDGE;
        case TextureWrap::Repeat: return GL_REPEAT;
    }

    AB_CORE_ASSERT(false, "Unknown texture wrap!");
    return 0;
}

OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, TextureFilter filter)
    : m_Width(width), m_Height(height), m_Format(format), m_Wrap(wrap), m_Filter(filter)
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([this]()
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, AmberToOpenGLTextureFilter(m_Filter));
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, AmberToOpenGLTextureFilter(m_Filter));

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, AmberToOpenGLTextureWrap(m_Wrap));
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, AmberToOpenGLTextureWrap(m_Wrap));

        GLenum type = m_Format == TextureFormat::Float16 ? GL_FLOAT : GL_UNSIGNED_BYTE;
        glTexImage2D(GL_TEXTURE_2D, 0, AmberToOpenGLInternalTextureFormat(m_Format), m_Width, m_Height, 0, AmberToOpenGLTextureFormat(m_Format), type, nullptr);
    });

    m_ImageData.Allocate(width * height * Texture::GetBPP(format));
}

OpenGLTexture2D::OpenGLTexture2D(const std::string& path, bool srgb, bool flip, TextureWrap wrap, TextureFilter filter)
    : m_Path(path), m_Wrap(wrap), m_Filter(filter)
{
    AB_PROFILE_FUNCTION();

    stbi_set_flip_vertically_on_load(flip);

    int width, height, channels;
    {
        AB_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std:string&)");
        m_ImageData.Data = (byte*)stbi_load(path.c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
    }
    AB_CORE_ASSERT(m_ImageData, "Failed to load image!");

    m_Width = width;
    m_Height = height;
    m_Format = channels == 3 ? TextureFormat::RGB : TextureFormat::RGBA;

    m_Loaded = true;

    RenderCommand::Submit([=]()
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, AmberToOpenGLTextureFilter(m_Filter));
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, AmberToOpenGLTextureFilter(m_Filter));

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, AmberToOpenGLTextureWrap(m_Wrap));
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, AmberToOpenGLTextureWrap(m_Wrap));
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_R, AmberToOpenGLTextureWrap(m_Wrap));

        GLenum type = m_Format == TextureFormat::Float16 ? GL_FLOAT : GL_UNSIGNED_BYTE;
        glTexImage2D(GL_TEXTURE_2D, 0, AmberToOpenGLInternalTextureFormat(m_Format), m_Width, m_Height, 0, AmberToOpenGLTextureFormat(m_Format), type, m_ImageData.Data);

        stbi_image_free(m_ImageData.Data);
    });
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        glDeleteTextures(1, &m_RendererID);
    });
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([this, slot]()
    {
        glBindTextureUnit(slot, m_RendererID);
    });
}

void OpenGLTexture2D::Lock()
{
    m_Locked = true;
}

void OpenGLTexture2D::Unlock()
{
    m_Locked = false;
    RenderCommand::Submit([this]()
    {
        GLenum type = m_Format == TextureFormat::Float16 ? GL_FLOAT : GL_UNSIGNED_BYTE;
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, AmberToOpenGLTextureFormat(m_Format), type, m_ImageData.Data);
    });
}

void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
{
    AB_CORE_ASSERT(m_Locked, "Texture must be locked!");
    m_ImageData.Allocate(width * height * Texture::GetBPP(m_Format));
}

Buffer& OpenGLTexture2D::GetWritableBuffer()
{
    AB_CORE_ASSERT(m_Locked, "Texture must be locked!");
    return m_ImageData;
}

}