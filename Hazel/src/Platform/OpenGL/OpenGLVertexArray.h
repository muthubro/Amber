#pragma once

#include "Hazel/Renderer/VertexArray.h"

namespace Hazel {

class OpenGLVertexArray : public VertexArray {
public:
	OpenGLVertexArray();
	~OpenGLVertexArray();

	void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

	inline const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer() const override { return m_VertexBuffers; }
	inline const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

	void Bind() const override;
	void Unbind() const override;

private:
	uint32_t m_RendererID;
	std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
	std::shared_ptr<IndexBuffer> m_IndexBuffer;
};

}
