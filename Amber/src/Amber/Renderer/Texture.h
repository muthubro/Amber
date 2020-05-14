#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/Buffer.h"
#include "Amber/Core/Core.h"

namespace Amber
{

    enum class TextureFormat
    {
        RGB, RGBA, Float16
    };

    enum class TextureWrap
    {
        Clamp, Repeat
    };

    enum class TextureFilter
    {
        Linear, Nearest
    };

    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual void Bind(uint32_t slot = 0) const = 0;

        virtual uint32_t GetRendererID() const = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual TextureFormat GetFormat() const = 0;

        static uint32_t GetBPP(TextureFormat format);

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

        static Ref<Texture2D> Create(TextureFormat format, uint32_t width, uint32_t height, TextureWrap wrap = TextureWrap::Clamp, TextureFilter filter = TextureFilter::Linear);
        static Ref<Texture2D> Create(const std::string& path, TextureWrap wrap = TextureWrap::Clamp, TextureFilter filter = TextureFilter::Linear);

        static Texture2DBounds GetBounds(const glm::vec2& position, const glm::vec2& cellSize, const glm::vec2& cellCount = glm::vec2(1.0f));
    };

}
