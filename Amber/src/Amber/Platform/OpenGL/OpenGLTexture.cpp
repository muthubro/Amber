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
        case TextureFormat::R:              return GL_RED;
        case TextureFormat::RG:             return GL_RG;
        case TextureFormat::RGB:            return GL_RGB;
        case TextureFormat::RGBA:           return GL_RGBA;
        case TextureFormat::Float16:        return GL_RGB;
        case TextureFormat::DepthStencil:   return GL_DEPTH_STENCIL;
    }

    AB_CORE_ASSERT(false, "Unknown texture format!");
    return 0;
}

static GLint AmberToOpenGLInternalTextureFormat(TextureFormat format, bool srgb = false)
{
    switch (format)
    {
        case TextureFormat::R:              return GL_R8;
        case TextureFormat::RG:             return GL_RG8;
        case TextureFormat::RGB:            return srgb ? GL_SRGB8 : GL_RGB8;
        case TextureFormat::RGBA:           return srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        case TextureFormat::Float16:        return GL_RGBA16F;
        case TextureFormat::DepthStencil:   return GL_DEPTH24_STENCIL8;
    }

    AB_CORE_ASSERT(false, "Unknown texture format!");
    return 0;
}

static GLenum AmberToOpenGLTextureFilter(TextureFilter filter, bool mipmap = false)
{
    switch (filter)
    {
        case TextureFilter::Linear:  return mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
        case TextureFilter::Nearest: return mipmap ? GL_NEAREST_MIPMAP_LINEAR : GL_NEAREST;
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

OpenGLTexture2D::OpenGLTexture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, TextureFilter filter, uint32_t samples)
    : m_Width(width), m_Height(height), m_Format(format), m_Wrap(wrap), m_Filter(filter)
{
    Ref<OpenGLTexture2D> instance = this;
    RenderCommand::Submit([instance, samples]() mutable {
        AB_PROFILE_FUNCTION();

        if (samples > 1)
        {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &instance->m_RendererID);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, instance->m_RendererID);
            glTextureStorage2DMultisample(
                instance->m_RendererID, samples, AmberToOpenGLInternalTextureFormat(instance->m_Format), 
                instance->m_Width, instance->m_Height, GL_FALSE);
        }
        else
        {
            glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_RendererID);
            glBindTexture(GL_TEXTURE_2D, instance->m_RendererID);

            glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, AmberToOpenGLTextureFilter(instance->m_Filter));
            glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, AmberToOpenGLTextureFilter(instance->m_Filter));
            glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_S, AmberToOpenGLTextureWrap(instance->m_Wrap));
            glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_T, AmberToOpenGLTextureWrap(instance->m_Wrap));

            glTextureStorage2D(
                instance->m_RendererID, 1, AmberToOpenGLInternalTextureFormat(instance->m_Format), 
                instance->m_Width, instance->m_Height);
        }
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

        if (stbi_is_hdr(path.c_str()))
        {
            AB_CORE_TRACE("Loading HDR texture {0}, srgb = {1}", path, srgb);

            m_ImageData.Data = (byte*)stbi_loadf(path.c_str(), &width, &height, &channels, 0);
            m_IsHDR = true;
            m_Format = TextureFormat::Float16;
        }
        else
        {
            AB_CORE_TRACE("Loading texture {0}, srgb = {1}", path, srgb);

            m_ImageData.Data = (byte*)stbi_load(path.c_str(), &width, &height, &channels, srgb ? STBI_rgb : STBI_rgb_alpha);
            m_Format = srgb ? TextureFormat::RGB : TextureFormat::RGBA;
        }
    }
    AB_CORE_ASSERT(m_ImageData, "Failed to load image!");

    m_Width = width;
    m_Height = height;

    m_Loaded = true;

    Ref<OpenGLTexture2D> instance = this;
    RenderCommand::Submit([instance, srgb]() mutable {
        glCreateTextures(GL_TEXTURE_2D, 1, &instance->m_RendererID);
        glBindTexture(GL_TEXTURE_2D, instance->m_RendererID);

        uint32_t levels = instance->GetMipLevelCount();

        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, AmberToOpenGLTextureFilter(instance->m_Filter, levels > 1));
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, AmberToOpenGLTextureFilter(instance->m_Filter));
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_S, AmberToOpenGLTextureWrap(instance->m_Wrap));
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_T, AmberToOpenGLTextureWrap(instance->m_Wrap));
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_R, AmberToOpenGLTextureWrap(instance->m_Wrap));

        GLenum type = instance->m_Format == TextureFormat::Float16 ? GL_FLOAT : GL_UNSIGNED_BYTE;
        glTexImage2D(
            GL_TEXTURE_2D, 0, AmberToOpenGLInternalTextureFormat(instance->m_Format, srgb), 
            instance->m_Width, instance->m_Height, 0, AmberToOpenGLTextureFormat(instance->m_Format), 
            type, instance->m_ImageData.Data);
        glGenerateTextureMipmap(instance->m_RendererID);

        instance->m_ImageData.Clear();
    });
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    RendererID rendererID = m_RendererID;
    RenderCommand::Submit([rendererID]() {
        AB_PROFILE_FUNCTION();

        glDeleteTextures(1, &rendererID);
    });
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
    Ref<const OpenGLTexture2D> instance = this;
    RenderCommand::Submit([instance, slot]() {
        AB_PROFILE_FUNCTION();

        glBindTextureUnit(slot, instance->m_RendererID);
    });
}

void OpenGLTexture2D::Lock()
{
    m_Locked = true;
}

void OpenGLTexture2D::Unlock()
{
    m_Locked = false;

    Ref<OpenGLTexture2D> instance = this;
    RenderCommand::Submit([instance]() {
        GLenum type =
            instance->m_Format == TextureFormat::Float16 ?
                GL_FLOAT :
                instance->m_Format == TextureFormat::DepthStencil ?
                    GL_UNSIGNED_INT_24_8 :
                    GL_UNSIGNED_BYTE;
        glTextureSubImage2D(
            instance->m_RendererID, 0, 0, 0, instance->m_Width, instance->m_Height, 
            AmberToOpenGLTextureFormat(instance->m_Format), type, instance->m_ImageData.Data);
    });
}

void OpenGLTexture2D::Resize(uint32_t width, uint32_t height)
{
    AB_CORE_ASSERT(m_Locked, "Texture must be locked!");
    m_ImageData.Allocate(width * height * Texture::GetBPP(m_Format));
}

void OpenGLTexture2D::CopyTo(Ref<Texture2D> dst)
{
    Ref<OpenGLTexture2D> instance = this;
    RenderCommand::Submit([instance, dst]() {
        glCopyImageSubData(instance->GetRendererID(), GL_TEXTURE_2D, 0, 0, 0, 0,
                           dst->GetRendererID(), GL_TEXTURE_2D, 0, 0, 0, 0,
                           instance->GetWidth(), instance->GetHeight(), 1);
    });
}

Buffer& OpenGLTexture2D::GetWritableBuffer()
{
    AB_CORE_ASSERT(m_Locked, "Texture must be locked!");
    return m_ImageData;
}

OpenGLTextureCube::OpenGLTextureCube(TextureFormat format, uint32_t width, uint32_t height)
    : m_Width(width), m_Height(height), m_Format(format)
{
    Ref<OpenGLTextureCube> instance = this;
    RenderCommand::Submit([instance]() mutable {
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &instance->m_RendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, instance->m_RendererID);

        uint32_t levels = instance->GetMipLevelCount();

        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, AmberToOpenGLTextureFilter(TextureFilter::Linear, levels > 1));
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        GLenum type = instance->m_Format == TextureFormat::Float16 ? GL_FLOAT : GL_UNSIGNED_BYTE;
        glTextureStorage2D(
            instance->m_RendererID, levels, AmberToOpenGLInternalTextureFormat(instance->m_Format), 
            instance->m_Width, instance->m_Height);
    });
}

OpenGLTextureCube::OpenGLTextureCube(const std::string& path)
{
    stbi_set_flip_vertically_on_load(false);

    int width, height, channels;
    m_ImageData = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

    m_Width = width;
    m_Height = height;
    m_Format = TextureFormat::RGB;

    uint32_t faceWidth = m_Width / 4;
    uint32_t faceHeight = m_Height / 3;

    std::array<byte*, 6> faces;
    for (uint32_t i = 0; i < 6; i++)
        faces[i] = new byte[faceWidth * faceHeight * 3];

    uint32_t faceIndex = 0;
    for (uint32_t i = 0; i < 4; i++)
    {
        for (uint32_t y = 0; y < faceHeight; y++)
        {
            uint32_t yOffset = y + faceHeight;
            for (uint32_t x = 0; x < faceWidth; x++)
            {
                uint32_t xOffset = x + i * faceWidth;

                faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 0];
                faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 1];
                faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 2];
            }
        }
        faceIndex++;
    }

    for (uint32_t i = 0; i < 3; i++)
    {
        if (i == 1)
            continue;

        for (uint32_t y = 0; y < faceHeight; y++)
        {
            uint32_t yOffset = y + i * faceHeight;
            for (uint32_t x = 0; x < faceWidth; x++)
            {
                uint32_t xOffset = x + faceWidth;

                faces[faceIndex][(x + y * faceWidth) * 3 + 0] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 0];
                faces[faceIndex][(x + y * faceWidth) * 3 + 1] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 1];
                faces[faceIndex][(x + y * faceWidth) * 3 + 2] = m_ImageData[(xOffset + yOffset * m_Width) * 3 + 2];
            }
        }
        faceIndex++;
    }

    Ref<OpenGLTextureCube> instance = this;
    RenderCommand::Submit([instance, faceWidth, faceHeight, faces]() mutable {
        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &instance->m_RendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, instance->m_RendererID);

        uint32_t levels = instance->GetMipLevelCount();

        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MIN_FILTER, AmberToOpenGLTextureFilter(TextureFilter::Linear, levels > 1));
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(instance->m_RendererID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        auto internalFormat = AmberToOpenGLInternalTextureFormat(instance->m_Format);
        auto format = AmberToOpenGLTextureFormat(instance->m_Format);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[2]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[0]);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[4]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[5]);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[1]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, faceWidth, faceHeight, 0, format, GL_UNSIGNED_BYTE, faces[3]);

        glGenerateTextureMipmap(instance->m_RendererID);

        for (uint32_t i = 0; i < 6; i++)
            delete[] faces[i];

        stbi_image_free(instance->m_ImageData);
    });
}

OpenGLTextureCube::~OpenGLTextureCube()
{
    RendererID rendererID = m_RendererID;
    RenderCommand::Submit([rendererID]() {
        AB_PROFILE_FUNCTION();

        glDeleteTextures(1, &rendererID);
    });
}

void OpenGLTextureCube::Bind(uint32_t slot) const
{
    Ref<const OpenGLTextureCube> instance = this;
    RenderCommand::Submit([instance, slot]() {
        glBindTextureUnit(slot, instance->m_RendererID);
    });
}

}