#pragma once

#include "Amber/Core/Buffer.h"

#include "Amber/Renderer/Buffer.h"

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

    const BufferLayout& GetLayout() const override { return m_Layout; }
    void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

    size_t GetSize() const override { return m_Size; }
    uint32_t GetRendererID() const override { return m_RendererID; }

private:
    uint32_t m_RendererID = 0;
    size_t m_Size;
    BufferLayout m_Layout;

    Buffer m_LocalData;
};

class OpenGLIndexBuffer : public IndexBuffer 
{
public:
    OpenGLIndexBuffer(void* data, size_t size);
    ~OpenGLIndexBuffer();

    void Bind() const override;
    void Unbind() const override;

    void SetData(void* buffer, size_t size, uint32_t offset = 0) override;
    
    size_t GetCount() const override { return (size_t)(m_Size / sizeof(uint32_t)); }
    size_t GetSize() const override { return m_Size; }
    uint32_t GetRendererID() const override { return m_RendererID; }

private:
    uint32_t m_RendererID = 0;
    size_t m_Size;

    Buffer m_LocalData;
};


}