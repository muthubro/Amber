#pragma once

#include "Amber/Renderer/Shader.h"
#include "Amber/Renderer/VertexBuffer.h"

namespace Amber
{

struct PipelineSpecification
{
    Ref<Shader> Shader;
    VertexBufferLayout Layout;
};

class Pipeline : public RefCounted
{
public:
    virtual ~Pipeline() = default;

    virtual void Bind() = 0;
    virtual void Invalidate() = 0;

    virtual PipelineSpecification& GetSpecification() = 0;
    virtual const PipelineSpecification& GetSpecification() const = 0;

    static Ref<Pipeline> Create(const PipelineSpecification& spec);
};

}