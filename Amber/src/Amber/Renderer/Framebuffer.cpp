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
        case RendererAPI::API::None:
            AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!");
            return nullptr;

        case RendererAPI::API::OpenGL:
            return CreateRef<OpenGLFramebuffer>(spec);
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

FramebufferPool* FramebufferPool::s_Instance = new FramebufferPool();

std::weak_ptr<Framebuffer> FramebufferPool::AllocateBuffer()
{
    return std::weak_ptr<Framebuffer>();
}

void FramebufferPool::Add(std::weak_ptr<Framebuffer> frameBuffer)
{
    m_Pool.push_back(frameBuffer);
}

}