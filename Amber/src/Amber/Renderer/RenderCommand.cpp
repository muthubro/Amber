#include "abpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Amber 
{

Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();

}