#include "abpch.h"
#include "Pipeline.h"

#include "Amber/Platform/OpenGL/OpenGLPipeline.h"

#include "Amber/Renderer/Renderer.h"

namespace Amber
{

Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::OpenGL:  return Ref<OpenGLPipeline>::Create(spec);
        case RendererAPI::API::None:    AB_CORE_ASSERT(false, "RendererAPI::None is not supported right now!"); return nullptr;
    }

    AB_CORE_ASSERT(false, "Unknown Renderer API");
    return nullptr;
}

}