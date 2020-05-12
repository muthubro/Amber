#pragma once

#include "Amber/Renderer/Buffer.h"

namespace Amber 
{

class OpenGLVertexBuffer : public VertexBuffer 
{
public:
    OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
    OpenGLVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
    ~OpenGLVertexBuffer();

    void Bind() const override;
    void Unbind() const override;

    void SetData(void* buffer, uint32_t size, uint32_t offset = 0) override;

    const BufferLayout& GetLayout() const override { return m_Layout; }
    void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

    uint32_t GetSize() const override { return 0; }
    RendererID GetRendererID() const override { return m_RendererID; }

private:
    RendererID m_RendererID;
    BufferLayout m_Layout;
};

class OpenGLIndexBuffer : public IndexBuffer 
{
public:
    OpenGLIndexBuffer(void* data, uint32_t count);
    ~OpenGLIndexBuffer();

    void Bind() const override;
    void Unbind() const override;

    void SetData(void* buffer, uint32_t size, uint32_t offset = 0) override;
    
    uint32_t GetCount() const override { return m_Count; }
    uint32_t GetSize() const override { return 0; }
    RendererID GetRendererID() const override { return m_RendererID; }

private:
    RendererID m_RendererID;
    uint32_t m_Count;
};


}