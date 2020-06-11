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

enum class DepthBufferType
{
    None = 0,
    Texture,
    Renderbuffer
};

struct FramebufferSpecification
{
    uint32_t Width = 1280;
    uint32_t Height = 720;
    FramebufferFormat Format;

    uint32_t ColorAttachmentCount = 1;
    DepthBufferType DepthAttachmentType = DepthBufferType::Renderbuffer;
    uint32_t Samples = 1;
    bool StencilBuffer = true;

    glm::vec4 ClearColor;
    
    bool SwapChainTarget = false;
};

class Framebuffer : public RefCounted
{
public:
    virtual ~Framebuffer() {}

    virtual void Reset() = 0;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual RendererID GetRendererID() const = 0;
    virtual const std::vector<Ref<Texture2D>>& GetColorAttachments() const = 0;
    virtual RendererID GetDepthAttachment() const = 0;

    virtual FramebufferSpecification& GetSpecification() = 0;
    virtual const FramebufferSpecification& GetSpecification() const = 0;

    static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
};

class FramebufferPool final
{
public:
    WeakRef<Framebuffer> AllocateBuffer();
    void Add(WeakRef<Framebuffer> frameBuffer);

    std::vector<WeakRef<Framebuffer>>& GetAll() { return m_Pool; }
    const std::vector<WeakRef<Framebuffer>>& GetAll() const { return m_Pool; }

    static FramebufferPool* GetGlobal() { return s_Instance; }

private:
    std::vector<WeakRef<Framebuffer>> m_Pool;

    static FramebufferPool* s_Instance;
};

}
