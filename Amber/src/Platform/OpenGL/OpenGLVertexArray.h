#pragma once

#include "Amber/Renderer/VertexArray.h"

namespace Amber 
{

class OpenGLVertexArray : public VertexArray 
{
public:
	OpenGLVertexArray();
	~OpenGLVertexArray();

	void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
	void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

	const std::vector<Ref<VertexBuffer>>& GetVertexBuffer() const override { return m_VertexBuffers; }
	const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

	void Bind() const override;
	void Unbind() const override;

private:
	uint32_t m_RendererID;
	uint32_t m_VertexBufferIndex = 0;
	std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	Ref<IndexBuffer> m_IndexBuffer;
};

}
