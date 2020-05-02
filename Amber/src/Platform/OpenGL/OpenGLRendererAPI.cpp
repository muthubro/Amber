#include "abpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Amber 
{

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
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
}

void OpenGLRendererAPI::SetViewport(int x, int y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void OpenGLRendererAPI::EnableBlending()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLRendererAPI::DisableBlending()
{
	glDisable(GL_BLEND);
}

void OpenGLRendererAPI::EnableDepthBuffer()
{
	glEnable(GL_DEPTH_TEST);
}

void OpenGLRendererAPI::DisableDepthBuffer()
{
	glDisable(GL_DEPTH_TEST);
}

void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
{
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

}