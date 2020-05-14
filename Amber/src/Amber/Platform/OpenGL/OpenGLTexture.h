#pragma once

#include <glad/glad.h>

#include "Amber/Renderer/Texture.h"

namespace Amber
{

class OpenGLTexture2D : public Texture2D
{
public:
    OpenGLTexture2D(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, TextureFilter filter);
    OpenGLTexture2D(const std::string& path, TextureWrap wrap, TextureFilter filter);
    ~OpenGLTexture2D();

    void Bind(uint32_t slot = 0) const override;

    void Lock() override;
    void Unlock() override;

    void Resize(uint32_t width, uint32_t height) override;

    bool Loaded() const override { return m_Loaded; }

    const std::string& GetPath() const override { return m_Path; }
    uint32_t GetRendererID() const override { return m_RendererID; }
    uint32_t GetWidth() const override { return m_Width; }
    uint32_t GetHeight() const override { return m_Height; }
    TextureFormat GetFormat() const override { return m_Format; }
    Buffer& GetWritableBuffer() override;

    bool operator==(const Texture& other) const override 
    { 
        return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID; 
    }

private:
    uint32_t m_RendererID = 0;

    std::string m_Path;
    uint32_t m_Width, m_Height;
    TextureFormat m_Format;
    TextureWrap m_Wrap;
    TextureFilter m_Filter;

    Buffer m_ImageData;

    bool m_Locked = false;
    bool m_Loaded = false;
};

}
