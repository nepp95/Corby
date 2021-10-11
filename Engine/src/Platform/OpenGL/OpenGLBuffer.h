#pragma once

#include "Engine/Renderer/Buffer.h"

namespace Engine {
	class OpenGLVertexBuffer : public VertexBuffer {
	public:
		OpenGLVertexBuffer(float* vertices, unsigned int size);
		virtual ~OpenGLVertexBuffer();

		virtual void bind() const;
		virtual void unbind() const;

	private:
		unsigned int m_rendererID;
	};

	class OpenGLIndexBuffer : public IndexBuffer {
	public:
		OpenGLIndexBuffer(unsigned int* indices, unsigned int count);
		virtual ~OpenGLIndexBuffer();

		virtual void bind() const;
		virtual void unbind() const;

		virtual unsigned int getCount() const { return m_count; }

	private:
		unsigned int m_rendererID;
		unsigned int m_count;
	};
}