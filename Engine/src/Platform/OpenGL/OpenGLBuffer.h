#pragma once

#include "Engine/Renderer/Buffer.h"

namespace Engine {
	class OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer(float* vertices, unsigned int size);
		virtual ~OpenGLVertexBuffer();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual const BufferLayout& getLayout() const override { return m_layout; }
		virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }

	private:
		unsigned int m_rendererID;
		BufferLayout m_layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer(unsigned int* indices, unsigned int count);
		virtual ~OpenGLIndexBuffer();

		virtual void bind() const override;
		virtual void unbind() const override;

		virtual unsigned int getCount() const override { return m_count; }

	private:
		unsigned int m_rendererID;
		unsigned int m_count;
	};
}