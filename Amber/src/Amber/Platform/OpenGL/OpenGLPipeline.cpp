#include "abpch.h"
#include "OpenGLPipeline.h"

#include <glad/glad.h>

#include "Amber/Renderer/RenderCommand.h"

namespace Amber
{

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::Float:		return GL_FLOAT;
        case ShaderDataType::Float2:	return GL_FLOAT;
        case ShaderDataType::Float3:	return GL_FLOAT;
        case ShaderDataType::Float4:	return GL_FLOAT;
        case ShaderDataType::Mat3:		return GL_FLOAT;
        case ShaderDataType::Mat4:		return GL_FLOAT;
        case ShaderDataType::Int:		return GL_INT;
        case ShaderDataType::Int2:		return GL_INT;
        case ShaderDataType::Int3:		return GL_INT;
        case ShaderDataType::Int4:		return GL_INT;
        case ShaderDataType::Bool:		return GL_BOOL;
    }

    AB_CORE_ASSERT(false, "Invalid shader data type!");
    return 0;
}

OpenGLPipeline::OpenGLPipeline(const PipelineSpecification& spec)
    : m_Specification(spec)
{
    Invalidate();
}

OpenGLPipeline::~OpenGLPipeline()
{
    RendererID rendererID = m_VertexArrayRendererID;
    RenderCommand::Submit([rendererID]() {
        glDeleteVertexArrays(1, &rendererID);
    });
}

void OpenGLPipeline::Bind()
{
    AB_CORE_ASSERT(m_VertexArrayRendererID != 0, "Invalid vertex array!");

    Ref<OpenGLPipeline> instance = this;
    RenderCommand::Submit([instance]() {
        AB_PROFILE_FUNCTION();

        glBindVertexArray(instance->m_VertexArrayRendererID);

        const auto& layout = instance->m_Specification.Layout;
        uint32_t attribIndex = 0;
        for (const auto& element : layout)
        {
            auto glBaseType = ShaderDataTypeToOpenGLBaseType(element.Type);
            glEnableVertexAttribArray(attribIndex);
            if (glBaseType == GL_INT)
            {
                glVertexAttribIPointer(attribIndex,
                                       element.GetComponentCount(),
                                       glBaseType,
                                       layout.GetStride(),
                                       (const void*)(intptr_t)element.Offset);
            }
            else
            {
                glVertexAttribPointer(attribIndex,
                                      element.GetComponentCount(),
                                      glBaseType,
                                      element.Normalized,
                                      layout.GetStride(),
                                      (const void*)(intptr_t)element.Offset);
            }
            attribIndex++;
        }
    });
}

void OpenGLPipeline::Invalidate()
{
    AB_CORE_ASSERT(!m_Specification.Layout.GetElements().empty(), "Vertex buffer layout cannot be empty!");

    Ref<OpenGLPipeline> instance = this;
    RenderCommand::Submit([instance]() mutable {
        AB_PROFILE_FUNCTION();

        auto& rendererID = instance->m_VertexArrayRendererID;
        if (rendererID)
            glDeleteVertexArrays(1, &rendererID);

        glCreateVertexArrays(1, &rendererID);
    });
}

}