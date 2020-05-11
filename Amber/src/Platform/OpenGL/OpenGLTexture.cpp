#include "abpch.h"
#include "OpenGLTexture.h"

#include <stb_image.h>

#include "Amber/Renderer/RenderCommand.h"

namespace Amber
{
OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
    : m_Width(width), m_Height(height)
{
    AB_PROFILE_FUNCTION();

    m_InternalFormat = GL_RGBA8;
    m_DataFormat = GL_RGBA;

    RenderCommand::Submit([=]()
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    });
}

OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
    : m_Path(path)
{
    AB_PROFILE_FUNCTION();

    RenderCommand::Submit([=]()
    {
        stbi_set_flip_vertically_on_load(1);

        int width, height, channels;
        uint8_t* data = nullptr;
        {
            AB_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std:string&)");
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }
        AB_CORE_ASSERT(data, "Failed to load image!");

        m_Width = width;
        m_Height = height;

        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }
        else if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        AB_CORE_ASSERT(internalFormat & dataFormat, "Texture format not supported!");

        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

        AB_CORE_TRACE("Loaded texture {}", path);

        stbi_image_free(data);
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

void OpenGLTexture2D::SetData(void* data, uint32_t size)
{
    AB_PROFILE_FUNCTION();

    uint8_t bpp = m_DataFormat == GL_RGB ? 3 : 4;
    AB_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data should be the full texture!");
    RenderCommand::Submit([=]()
    {
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
    });
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
    AB_PROFILE_FUNCTION();
    
    RenderCommand::Submit([=]()
    {
        glBindTextureUnit(slot, m_RendererID);
    });
}

}