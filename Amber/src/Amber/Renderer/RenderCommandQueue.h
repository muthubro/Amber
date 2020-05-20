#pragma once

#include "Amber/Core/Base.h"

namespace Amber
{

class RenderCommandQueue
{
public:
    typedef void(*RenderCommandFn)(void*);

    RenderCommandQueue();
    ~RenderCommandQueue();

    void* Allocate(RenderCommandFn func, size_t size);
    void Execute();

private:
    byte* m_CommandBuffer;
    byte* m_CommandBufferPtr;
    uint32_t m_CommandCount = 0;
};

}
