#include "abpch.h"
#include "OpenGLVertexArray.h"

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

OpenGLVertexArray::OpenGLVertexArray()
{
    Ref<OpenGLVertexArray> instance = this;
    RenderCommand::Submit([instance]() mutable {
        AB_PROFILE_FUNCTION();

        glCreateVertexArrays(1, &instance->m_RendererID);
    });
}

OpenGLVertexArray::~OpenGLVertexArray()
{
    RendererID rendererID = m_RendererID;
    RenderCommand::Submit([rendererID]() {
        AB_PROFILE_FUNCTION();

        glDeleteVertexArrays(1, &rendererID);
    });
}

void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
    AB_PROFILE_FUNCTION();

    AB_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex buffer layout not set!");

    Bind();
    vertexBuffer->Bind();
    m_VertexBuffers.push_back(vertexBuffer);

    Ref<OpenGLVertexArray> instance = this;
    RenderCommand::Submit([instance, vertexBuffer]() mutable {
        const auto & layout = vertexBuffer->GetLayout();
        for (const auto& element : layout)
        {
            glEnableVertexAttribArray(instance->m_VertexBufferIndex);

            auto glBaseType = ShaderDataTypeToOpenGLBaseType(element.Type);
            if (glBaseType == GL_INT)
            {
                glVertexAttribIPointer(instance->m_VertexBufferIndex,
                    element.GetComponentCount(),
                    glBaseType,
                    layout.GetStride(),
                    (const void*)element.Offset);
            }
            else
            {
                glVertexAttribPointer(instance->m_VertexBufferIndex,
                    element.GetComponentCount(),
                    glBaseType,
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)element.Offset);
            }
            instance->m_VertexBufferIndex++;
        }
    });
}

void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
    m_IndexBuffer = indexBuffer;
}

void OpenGLVertexArray::Bind() const
{
    Ref<const OpenGLVertexArray> instance = this;
    RenderCommand::Submit([instance]() {
        AB_PROFILE_FUNCTION();

        glBindVertexArray(instance->m_RendererID);
    });

    if (m_IndexBuffer)
        m_IndexBuffer->Bind();
}

void OpenGLVertexArray::Unbind() const
{
    RenderCommand::Submit([]() {
        AB_PROFILE_FUNCTION();

        glBindVertexArray(0);
    });
}

}