#include "abpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

#include "Amber/Renderer/RenderCommand.h"

namespace Amber 
{

static GLenum ComparisonFuncToGLFunc(ComparisonFunc func)
{
    switch (func)
    {
        case ComparisonFunc::Never:          return GL_NEVER;
        case ComparisonFunc::Less:           return GL_LESS;
        case ComparisonFunc::LessEqual:      return GL_LEQUAL;
        case ComparisonFunc::Greater:        return GL_GREATER;
        case ComparisonFunc::GreaterEqual:   return GL_GEQUAL;
        case ComparisonFunc::Equal:          return GL_EQUAL;
        case ComparisonFunc::NotEqual:       return GL_NOTEQUAL;
        case ComparisonFunc::Always:         return GL_ALWAYS;
    }
    AB_CORE_ASSERT(false, "Unknown comparison function!");
    return 0;
}

static GLenum PrimitiveTypeToGLType(PrimitiveType type)
{
    switch (type)
    {
        case PrimitiveType::Triangles:  return GL_TRIANGLES;
        case PrimitiveType::Lines:      return GL_LINES;
    }
    AB_CORE_ASSERT(false, "Unknown primitive type!");
    return 0;
}

static GLenum RasterizationModeToGLMode(RasterizationMode mode)
{
    switch (mode)
    {
        case RasterizationMode::Fill:   return GL_FILL;
        case RasterizationMode::Line:   return GL_LINE;
        case RasterizationMode::Point:  return GL_POINT;
    }
    AB_CORE_ASSERT(false, "Unknown rasterization mode!");
    return 0;
}

static GLenum StencilOperationToGLOperation(StencilOperation op)
{
    switch (op)
    {
        case StencilOperation::Keep:     return GL_KEEP;
        case StencilOperation::Zero:     return GL_ZERO;
        case StencilOperation::Replace:  return GL_REPLACE;
        case StencilOperation::Incr:     return GL_INCR;
        case StencilOperation::IncrWrap: return GL_INCR_WRAP;
        case StencilOperation::Decr:     return GL_DECR;
        case StencilOperation::DecrWrap: return GL_DECR_WRAP;
        case StencilOperation::Invert:   return GL_INVERT;
    }
    AB_CORE_ASSERT(false, "Unknown stencil operation!");
    return 0;
}

void OpenGLMessageCallback(
    uint32_t source,
    uint32_t type,
    uint32_t id,
    uint32_t severity,
    int length,
    const char* message,
    const void* userParam)
{
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:		 AB_CORE_CRITICAL(message); return;
        case GL_DEBUG_SEVERITY_MEDIUM:		 AB_CORE_ERROR(message); return;
        case GL_DEBUG_SEVERITY_LOW:			 AB_CORE_WARN(message); return;
        case GL_DEBUG_SEVERITY_NOTIFICATION: AB_CORE_TRACE(message); return;
    }

    AB_CORE_ASSERT(false, "Unknown severity level!");
}

void OpenGLRendererAPI::Init()
{
    AB_PROFILE_FUNCTION();

#ifdef AB_DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
#endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    auto& caps = RendererAPI::GetCapabilities();
    caps.Vendor = (const char*)glGetString(GL_VENDOR);
    caps.Renderer = (const char*)glGetString(GL_RENDERER);
    caps.Version = (const char*)glGetString(GL_VERSION);
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &caps.MaxColorAttachments);
    glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &caps.MaxTextureSamples);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &caps.MaxTextureSlots);

    GLenum error = glGetError();
    while (error != GL_NO_ERROR)
    {
        AB_CORE_ERROR("OpenGL Error {0}", error);
        error = glGetError();
    }
}

void OpenGLRendererAPI::SetViewport(int x, int y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}

void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRendererAPI::SetLineThickness(float thickness)
{
    glLineWidth(thickness);
}

void OpenGLRendererAPI::SetPointSize(float size)
{
    glPointSize(size);
}

void OpenGLRendererAPI::SetRasterizationMode(RasterizationMode mode)
{
    glPolygonMode(GL_FRONT_AND_BACK, RasterizationModeToGLMode(mode));
}

void OpenGLRendererAPI::SetStencilFunction(ComparisonFunc func, uint8_t ref, uint8_t mask)
{
    glStencilFunc(ComparisonFuncToGLFunc(func), ref, mask);
}

void OpenGLRendererAPI::SetStencilMask(uint8_t mask)
{
    glStencilMask(mask);
}

void OpenGLRendererAPI::SetStencilOperation(StencilOperation stencilFail, StencilOperation depthFail, StencilOperation depthPass)
{
    glStencilOp(StencilOperationToGLOperation(stencilFail),
                StencilOperationToGLOperation(depthFail),
                StencilOperationToGLOperation(depthPass));
}

void OpenGLRendererAPI::DrawIndexed(uint32_t indexCount, PrimitiveType type, bool depthTest, bool stencilTest)
{
    if (indexCount == 0)
        return;

    if (!depthTest)
        glDisable(GL_DEPTH_TEST);
    if (!stencilTest)
        glDisable(GL_STENCIL_TEST);

    GLenum primitiveType = PrimitiveTypeToGLType(type);
    glDrawElements(primitiveType, indexCount, GL_UNSIGNED_INT, nullptr);

    if (!depthTest)
        glEnable(GL_DEPTH_TEST);
    if (!stencilTest)
        glEnable(GL_STENCIL_TEST);
}

void OpenGLRendererAPI::DrawIndexedOffset(uint32_t indexCount, PrimitiveType type, void* indexBufferPointer, uint32_t offset, bool depthTest, bool stencilTest)
{
    if (indexCount == 0)
        return;

    if (!depthTest)
        glDisable(GL_DEPTH_TEST);
    if (!stencilTest)
        glDisable(GL_STENCIL_TEST);

    GLenum primitiveType = PrimitiveTypeToGLType(type);
    glDrawElementsBaseVertex(primitiveType, indexCount, GL_UNSIGNED_INT, indexBufferPointer, offset);

    if (!depthTest)
        glEnable(GL_DEPTH_TEST);
    if (!stencilTest)
        glEnable(GL_STENCIL_TEST);
}

}