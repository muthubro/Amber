#pragma once

#include "Hazel/Renderer/Buffer.h"

namespace Hazel {

class OpenGLVertexBuffer : public VertexBuffer {
public:
	OpenGLVertexBuffer(float* vertices, uint32_t size, bool dynamic);
	~OpenGLVertexBuffer();

	void Bind() const override;
	void Unbind() const override;

	inline const BufferLayout& GetLayout() const override { return m_Layout; }
	inline void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

private:
	uint32_t m_RendererID;
	BufferLayout m_Layout;
};

class OpenGLIndexBuffer : public IndexBuffer {
public:
	OpenGLIndexBuffer(uint32_t* indices, uint32_t count, bool dynamic);
	~OpenGLIndexBuffer();

	void Bind() const override;
	void Unbind() const override;

	inline uint32_t GetCount() const override { return m_Count; }

private:
	uint32_t m_RendererID;
	uint32_t m_Count;
};


}