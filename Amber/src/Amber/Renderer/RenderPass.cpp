#include "abpch.h"
#include "RenderPass.h"

#include "Amber/Renderer/Renderer.h"

#include "Amber/Platform/OpenGL/OpenGLRenderPass.h"

namespace Amber
{

Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return Ref<OpenGLRenderPass>::Create(spec);
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

}