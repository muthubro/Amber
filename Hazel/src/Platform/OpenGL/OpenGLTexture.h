#pragma once

#include <glad/glad.h>

#include "Hazel/Renderer/Texture.h"

namespace Hazel
{

class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(uint32_t width, uint32_t height);
	OpenGLTexture2D(const std::string& path);
	~OpenGLTexture2D();

	inline uint32_t GetWidth() const override { return m_Width; }
	inline uint32_t GetHeight() const override { return m_Height; }

	void SetData(void* data, uint32_t size) override;

	void Bind(uint32_t slot = 0) const override;

	bool operator==(const Texture& other) const override 
	{ 
		return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID; 
	}

private:
	std::string m_Path;
	uint32_t m_RendererID;
	uint32_t m_Width, m_Height;
	GLenum m_InternalFormat, m_DataFormat;
};

}
