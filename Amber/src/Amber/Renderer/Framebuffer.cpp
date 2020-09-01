#include "abpch.h"
#include "Framebuffer.h"

#include "Amber/Renderer/Renderer.h"

#include "Amber/Platform/OpenGL/OpenGLFramebuffer.h"

namespace Amber
{

Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return Ref<OpenGLFramebuffer>::Create(spec);
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

FramebufferPool* FramebufferPool::s_Instance = new FramebufferPool();

WeakRef<Framebuffer> FramebufferPool::AllocateBuffer()
{
    return WeakRef<Framebuffer>();
}

void FramebufferPool::Add(WeakRef<Framebuffer> frameBuffer)
{
    m_Pool.push_back(frameBuffer);
}

}