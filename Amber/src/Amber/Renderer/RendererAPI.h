#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "Amber/Renderer/VertexArray.h"

namespace Amber 
{

struct RenderAPICapabilities
{
    std::string Vendor;
    std::string Renderer;
    std::string Version;

    int MaxColorAttachments;
    int MaxTextureSamples;
    int MaxTextureSlots;
};

enum class PrimitiveType
{
    Triangles, Lines
};

class RendererAPI 
{
public:
    enum class API 
    {
        None = 0, OpenGL
    };

    virtual void Init() = 0;

    virtual void SetViewport(int x, int y, uint32_t width, uint32_t height) = 0;

    virtual void SetClearColor(const glm::vec4& color) = 0;
    virtual void Clear() = 0;

    virtual void SetLineThickness(float thickness) = 0;

    virtual void DrawIndexed(uint32_t indexCount, PrimitiveType type, bool depthTest = true) = 0;
    virtual void DrawIndexedOffset(uint32_t indexCount, PrimitiveType type, void* indexBufferPointer, uint32_t offset, bool depthTest = true) = 0;

    static RenderAPICapabilities& GetCapabilities()
    {
        static RenderAPICapabilities capabilities;
        return capabilities;
    }
    static API GetAPI() { return s_API; }
    static Scope<RendererAPI> Create();

private:
    static API s_API;
};

}
