#pragma once

#include "Amber/Core/Base.h"
#include "Amber/Core/Buffer.h"

#include "Amber/Renderer/VertexBuffer.h"

namespace Amber 
{

class OpenGLVertexBuffer : public VertexBuffer 
{
public:
    OpenGLVertexBuffer(size_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
    OpenGLVertexBuffer(void* data, size_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
    ~OpenGLVertexBuffer();

    void Bind() const override;
    void Unbind() const override;

    void SetData(void* buffer, size_t size, uint32_t offset = 0) override;

    const VertexBufferLayout& GetLayout() const override { return m_Layout; }
    void SetLayout(const VertexBufferLayout& layout) override { m_Layout = layout; }

    size_t GetSize() const override { return m_Size; }
    RendererID GetRendererID() const override { return m_RendererID; }

private:
    RendererID m_RendererID = 0;
    size_t m_Size;
    VertexBufferLayout m_Layout;

    Buffer m_LocalData;
};

}