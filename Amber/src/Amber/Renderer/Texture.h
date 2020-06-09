#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/Buffer.h"
#include "Amber/Core/Base.h"

namespace Amber
{

enum class TextureFormat
{
    None = 0,
    RGB,
    RGBA,
    Float16,
    DepthStencil
};

enum class TextureWrap
{
    Clamp, Repeat
};

enum class TextureFilter
{
    Linear, Nearest
};

class Texture : public RefCounted
{
public:
    virtual ~Texture() = default;

    virtual void Bind(uint32_t slot = 0) const = 0;

    virtual uint32_t GetRendererID() const = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual TextureFormat GetFormat() const = 0;
    virtual uint32_t GetMipLevelCount() const = 0;

    static uint32_t GetBPP(TextureFormat format);
    static uint32_t CalculateMipMapCount(uint32_t width, uint32_t height);

    virtual bool operator==(const Texture& other) const = 0;
};

struct Texture2DBounds
{
    glm::vec2 minPos, maxPos;
};

class Texture2D : public Texture
{
public:
    virtual void Lock() = 0;
    virtual void Unlock() = 0;

    virtual void Resize(uint32_t width, uint32_t height) = 0;

    virtual bool Loaded() const = 0;

    virtual const std::string& GetPath() const = 0;
    virtual Buffer& GetWritableBuffer() = 0;

    static Ref<Texture2D> Create(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap = TextureWrap::Clamp, TextureFilter filter = TextureFilter::Linear, uint32_t samples = 1);
    static Ref<Texture2D> Create(const std::string& path, bool srgb = false, bool flip = false, TextureWrap wrap = TextureWrap::Clamp, TextureFilter filter = TextureFilter::Linear);

    static Texture2DBounds GetBounds(const glm::vec2& position, const glm::vec2& cellSize, const glm::vec2& cellCount = glm::vec2(1.0f));
};

class TextureCube : public Texture
{
public:
    virtual const std::string& GetPath() const = 0;

    static Ref<TextureCube> Create(TextureFormat format, uint32_t width, uint32_t height);
    static Ref<TextureCube> Create(const std::string& path);
};

}
