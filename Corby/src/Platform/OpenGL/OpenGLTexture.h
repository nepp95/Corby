#pragma once

#include "Engine/Renderer/Texture.h"

#include <glad/glad.h>

namespace Engine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height);
		OpenGLTexture2D(const std::string& filepath);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_width; }
		virtual uint32_t GetHeight() const override { return m_height; }
		virtual uint32_t GetRendererID() const override { return m_rendererID; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t slot = 0) const override;

		virtual bool IsLoaded() const override { return m_isLoaded; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_rendererID == ((OpenGLTexture2D&) other).m_rendererID;
		}

	private:
		std::string m_path;
		bool m_isLoaded = false;
		uint32_t m_width, m_height;
		uint32_t m_rendererID;
		GLenum m_internalFormat, m_dataFormat;
	};
}
