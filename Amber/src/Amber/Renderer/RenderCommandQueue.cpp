#include "abpch.h"
#include "RenderCommandQueue.h"

namespace Amber
{

static const uint32_t s_BufferSize = 10 * 1024 * 1024; // 10 MB

RenderCommandQueue::RenderCommandQueue()
{
    m_CommandBuffer = new byte[s_BufferSize];
    m_CommandBufferPtr = m_CommandBuffer;
    memset(m_CommandBuffer, 0, s_BufferSize);
}

RenderCommandQueue::~RenderCommandQueue()
{
    delete[] m_CommandBuffer;
}

void* RenderCommandQueue::Allocate(RenderCommandFn func, size_t size)
{
    *(RenderCommandFn*)m_CommandBufferPtr = func;
    m_CommandBufferPtr += sizeof(func);

    *(size_t*)m_CommandBufferPtr = size;
    m_CommandBufferPtr += sizeof(size_t);

    void* memory = m_CommandBufferPtr;
    m_CommandBufferPtr += size;

    m_CommandCount++;
    return memory;
}

void RenderCommandQueue::Execute()
{
    byte* buffer = m_CommandBuffer;
    for (uint32_t i = 0; i < m_CommandCount; i++)
    {
        auto function = *(RenderCommandFn*)buffer;
        buffer += sizeof(RenderCommandFn);

        size_t size = *(size_t*)buffer;
        buffer += sizeof(size_t);

        function(buffer);

        buffer += size;
    }

    m_CommandBufferPtr = m_CommandBuffer;
    m_CommandCount = 0;
}

}