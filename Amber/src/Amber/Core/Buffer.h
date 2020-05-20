#pragma once

#include "Amber/Core/Base.h"

namespace Amber
{

struct Buffer
{
    byte* Data;
    size_t Size;

    Buffer()
        : Data(nullptr), Size(0) {}

    Buffer(byte* data, size_t size)
        : Data(data), Size(size) {}

    void Allocate(size_t size)
    {
        delete[] Data;
        Data = nullptr;

        if (size == 0)
            return;

        Data = new byte[size];
        Size = size;
    }

    void ZeroInitialize()
    {
        if (Data)
            memset(Data, 0, Size);
    }

    void Write(void* data, size_t size, uint32_t offset = 0)
    {
        AB_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
        memcpy(Data + offset, data, size);
    }

    size_t GetSize() const { return Size; }

    static Buffer Copy(void* data, size_t size)
    {
        Buffer buffer;
        buffer.Allocate(size);
        buffer.Write(data, size);
        return buffer;
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