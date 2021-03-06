#include "abpch.h"
#include "Texture.h"

#include "Amber/Renderer/Renderer.h"

#include "Amber/Platform/OpenGL/OpenGLTexture.h"

namespace Amber
{

Ref<Texture2D> Texture2D::Create(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap, TextureFilter filter, uint32_t samples)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return Ref<OpenGLTexture2D>::Create(format, width, height, wrap, filter, samples);
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

Ref<Texture2D> Texture2D::Create(const std::string& path, bool srgb, bool flip, TextureWrap wrap, TextureFilter filter)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return Ref<OpenGLTexture2D>::Create(path, srgb, flip, wrap, filter);
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

Ref<TextureCube> TextureCube::Create(TextureFormat format, uint32_t width, uint32_t height)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return Ref<OpenGLTextureCube>::Create(format, width, height);
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

Ref<TextureCube> TextureCube::Create(const std::string& path)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return Ref<OpenGLTextureCube>::Create(path);
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

uint32_t Texture::GetBPP(TextureFormat format)
{
    switch (format)
    {
        case TextureFormat::RGB: return 3;
        case TextureFormat::RGBA: return 4;
    }

    return 0;
}

uint32_t Texture::CalculateMipMapCount(uint32_t width, uint32_t height)
{
    uint32_t levels = 1;
    while ((width | height) >> levels)
        levels++;

    return levels;
}

Texture2DBounds Texture2D::GetBounds(const glm::vec2& pos, const glm::vec2& cellSize, const glm::vec2& cellCount)
{
    return Texture2DBounds{ pos, pos + cellCount * cellSize };
}

}
