#include "abpch.h"
#include "RenderCommand.h"

#include "Amber/Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Amber 
{

Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}