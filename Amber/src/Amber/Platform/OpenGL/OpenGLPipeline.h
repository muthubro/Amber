#pragma once

#include "Amber/Renderer/Pipeline.h"

namespace Amber
{

class OpenGLPipeline : public Pipeline
{
public:
    OpenGLPipeline(const PipelineSpecification& spec);
    ~OpenGLPipeline();

    void Bind();
    void Invalidate();

    PipelineSpecification& GetSpecification() { return m_Specification; }
    const PipelineSpecification& GetSpecification() const { return m_Specification; }

private:
    PipelineSpecification m_Specification;
    RendererID m_VertexArrayRendererID = 0;
};

}