#pragma once

#include <vector>
#include <memory>

#include "Amber/Renderer/Buffer.h"

namespace Amber 
{

class VertexArray : public RefCounted
{
public:
    virtual ~VertexArray() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
    virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

    virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

    static Ref<VertexArray> Create();
};

}
