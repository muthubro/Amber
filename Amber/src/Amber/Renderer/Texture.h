#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/Core.h"

namespace Amber
{

    struct Texture2DBounds
    {
        glm::vec2 minPos, maxPos;
    };

    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void SetData(void* data, uint32_t size) = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;

        virtual bool operator==(const Texture& other) const = 0;
    };

    class Texture2D : public Texture
    {
    public:
        static Ref<Texture2D> Create(uint32_t width, uint32_t height);
        static Ref<Texture2D> Create(const std::string& path);

        static Texture2DBounds GetBounds(const glm::vec2& position, const glm::vec2& cellSize, const glm::vec2& cellCount = glm::vec2(1.0f));
    };

}
