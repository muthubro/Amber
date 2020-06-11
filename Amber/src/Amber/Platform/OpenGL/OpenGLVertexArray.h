#pragma once

#include "Amber/Renderer/VertexArray.h"

namespace Amber 
{

class OpenGLVertexArray : public VertexArray 
{
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray();

    void Bind() const override;
    void Unbind() const override;

    void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
    void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

    RendererID GetRendererID() { return m_RendererID; }
    const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

private:
    RendererID m_RendererID;
    uint32_t m_VertexBufferIndex = 0;
    Ref<IndexBuffer> m_IndexBuffer = nullptr;
    std::vector<Ref<VertexBuffer>> m_VertexBuffers;
};

}
