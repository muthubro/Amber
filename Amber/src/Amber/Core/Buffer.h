#pragma once

#include "Amber/Core/Base.h"

namespace Amber
{

struct Buffer
{
    byte* Data = nullptr;
    size_t Size = 0;

    Buffer() = default;

    Buffer(void* data, size_t size)
    {
        Allocate(size);
        Write(data, size);
    }

    Buffer(const Buffer& buffer)
    {
        Allocate(buffer.Size);
        Write(buffer.Data, Size);
    }

    Buffer(Buffer&& buffer) noexcept
    {
        Data = buffer.Data;
        Size = buffer.Size;

        buffer.Data = nullptr;
        buffer.Size = 0;
    }

    ~Buffer()
    {
        if (Data)
            delete[] Data;
    }

    void Allocate(size_t size)
    {
        delete[] Data;
        Data = nullptr;

        if (size == 0)
            return;

        Data = new byte[size];
        Size = size;
    }

    void Clear()
    {
        if (Data)
            delete[] Data;
        Data = nullptr;
        Size = 0;
    }

    void ZeroInitialize()
    {
        if (Data)
            memset(Data, 0, Size);
    }

    void Read(void* dest, size_t size, size_t offset = 0) const
    {
        AB_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
        memcpy(dest, Data + offset, size);
    }

    void Write(void* data, size_t size, size_t offset = 0)
    {
        AB_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
        memcpy(Data + offset, data, size);
    }

    size_t GetSize() const { return Size; }

    Buffer& operator=(const Buffer& other)
    {
        Allocate(other.Size);
        Write(other.Data, Size);
    }

    Buffer& operator=(Buffer&& other) noexcept
    {
        Data = other.Data;
        Size = other.Size;

        other.Data = nullptr;
        other.Size = 0;

        return *this;
    }

    operator bool() const
    {
        return Data;
    }

    byte& operator[](int index)
    {
        return Data[index];
    }

    byte operator[](int index) const
    {
        return Data[index];
    }

    template<typename T>
    T* As()
    {
        return (T*)Data;
    }
};

}