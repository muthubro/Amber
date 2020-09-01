#pragma once

#include "Amber/Core/Base.h"
#include "Amber/Core/Buffer.h"

#include "Amber/Renderer/IndexBuffer.h"

namespace Amber
{

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
    RendererID GetRendererID() const override { return m_RendererID; }

private:
    RendererID m_RendererID = 0;
    size_t m_Size;

    Buffer m_LocalData;
};

}