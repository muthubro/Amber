#include "abpch.h"
#include "RenderCommand.h"

#include "Amber/Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Amber 
{

Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

void RenderCommand::DrawIndexed(uint32_t indexCount)
{
    Submit([=]() {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    });
}

}