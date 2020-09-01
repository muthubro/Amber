#pragma once

namespace Amber
{

class IndexBuffer : public RefCounted
{
public:
    virtual ~IndexBuffer() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void SetData(void* buffer, size_t size, uint32_t offset = 0) = 0;

    virtual size_t GetCount() const = 0;
    virtual size_t GetSize() const = 0;
    virtual uint32_t GetRendererID() const = 0;

    static Ref<IndexBuffer> Create(void* data, size_t size);
};

}