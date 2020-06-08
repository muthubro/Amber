#pragma once

#include <glm/glm.hpp>

#include "Amber/Core/Base.h"

#include "Amber/Renderer/Texture.h"

namespace Amber
{

enum class FramebufferFormat
{
    None = 0,
    RGBA8,
    RGBA16F
};

struct FramebufferSpecification
{
    uint32_t Width = 1280;
    uint32_t Height = 720;
    glm::vec4 ClearColor;
    FramebufferFormat Format;
    uint32_t Samples = 1;
    bool SwapChainTarget = false;
};

class Framebuffer
{
public:
    virtual ~Framebuffer() {}

    virtual void Resize(uint32_t width, uint32_t height, bool forceRecreate = false) = 0;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void BindTexture(uint32_t slot = 0) const = 0;

    virtual RendererID GetRendererID() const = 0;
    virtual Ref<Texture2D> GetColorAttachment() const = 0;
    virtual Ref<Texture2D> GetDepthAttachment() const = 0;

    virtual const FramebufferSpecification& GetSpecification() const = 0;

    static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
};

class FramebufferPool final
{
public:
    std::weak_ptr<Framebuffer> AllocateBuffer();
    void Add(std::weak_ptr<Framebuffer> frameBuffer);

    const std::vector<std::weak_ptr<Framebuffer>>& GetAll() const { return m_Pool; }

    static FramebufferPool* GetGlobal() { return s_Instance; }

private:
    std::vector<std::weak_ptr<Framebuffer>> m_Pool;

    static FramebufferPool* s_Instance;
};

}
