#pragma once

#include <memory>
#include <string>

#include <glm/glm.hpp>

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

enum class ComparisonFunc
{
    Never, Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual, Always
};

enum class PrimitiveType
{
    Triangles, Lines
};

enum class RasterizationMode
{
    Fill, Line, Point
};

enum class StencilOperation
{
    Keep, Zero, Replace, Incr, IncrWrap, Decr, DecrWrap, Invert
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
    virtual void SetPointSize(float size) = 0;
    virtual void SetRasterizationMode(RasterizationMode mode) = 0;
    virtual void SetStencilFunction(ComparisonFunc func, uint8_t ref, uint8_t mask) = 0;
    virtual void SetStencilMask(uint8_t mask) = 0;
    virtual void SetStencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation depthPass) = 0;

    virtual void DrawIndexed(uint32_t indexCount, PrimitiveType type, bool depthTest = true, bool stencilTest = false) = 0;
    virtual void DrawIndexedOffset(uint32_t indexCount, PrimitiveType type, void* indexBufferPointer, uint32_t offset, bool depthTest = true, bool stencilTest = false) = 0;

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
