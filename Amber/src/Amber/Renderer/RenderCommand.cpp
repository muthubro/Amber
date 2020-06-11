#include "abpch.h"
#include "RenderCommand.h"

#include <glad/glad.h>

#include "Amber/Platform/OpenGL/OpenGLRendererAPI.h"

namespace Amber 
{

Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}