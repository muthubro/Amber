#pragma once

#include "Hazel/Renderer/RendererAPI.h"

namespace Hazel 
{

class RenderCommand 
{
public:
	static void Init() { s_RendererAPI->Init(); }

	static void SetViewPort(int x, int y, uint32_t width, uint32_t height) { s_RendererAPI->SetViewport(x, y, width, height); }

	static void EnableBlending() { s_RendererAPI->EnableBlending(); }
	static void DisableBlending() { s_RendererAPI->DisableBlending(); }

	static void EnableDepthBuffer() { s_RendererAPI->EnableDepthBuffer(); }
	static void DisableDepthBuffer() { s_RendererAPI->DisableDepthBuffer(); }

	static void SetClearColor(const glm::vec4& color) { s_RendererAPI->SetClearColor(color); }
	static void Clear() { s_RendererAPI->Clear(); }
	
	static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) { s_RendererAPI->DrawIndexed(vertexArray, indexCount); }

private:
	static Scope<RendererAPI> s_RendererAPI;
};

}
