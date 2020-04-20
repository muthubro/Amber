#pragma once

#include "Hazel/Core.h"

namespace Hazel 
{

enum class ShaderDataType 
{
	None = 0,
	Float, Float2, Float3, Float4,
	Mat3, Mat4,
	Int, Int2, Int3, Int4,
	Bool
};

struct BufferElement 
{
	std::string Name;
	ShaderDataType Type;
	uint32_t Size, Offset;
	bool Normalized;

	BufferElement(ShaderDataType type, const std::string& name, bool normalized = false);

	uint8_t GetComponentCount() const;
};

class BufferLayout 
{
public:
	BufferLayout() = default;

	BufferLayout(const std::initializer_list<BufferElement>& elements);

	inline uint32_t GetStride() const { return m_Stride; }
	inline std::vector<BufferElement> GetElements() const { return m_Elements; }

	inline std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
	inline std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
	inline std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
	inline std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

private:
	void CalculateOffsetsAndStride();

	std::vector<BufferElement> m_Elements;
	uint32_t m_Stride;
};

class VertexBuffer 
{
public:
	virtual ~VertexBuffer() = default;

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual const BufferLayout& GetLayout() const = 0;
	virtual void SetLayout(const BufferLayout& layout) = 0;

	static Ref<VertexBuffer> Create(float* vertices, uint32_t size, bool dynamic = false);
};

class IndexBuffer 
{
public:
	virtual ~IndexBuffer() {}

	virtual void Bind() const = 0;
	virtual void Unbind() const = 0;

	virtual uint32_t GetCount() const = 0;

	static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count, bool dynamic = false);
};

}
