#pragma once

#include "Engine/Renderer/VertexArray.h"

namespace Engine
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual	~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_vertexBuffers; }
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return m_indexBuffer; }

	private:
		uint32_t m_rendererID;
		uint32_t m_vertexBufferIndex = 0;
		std::vector<Ref<VertexBuffer>> m_vertexBuffers;
		Ref<IndexBuffer> m_indexBuffer;
	};
}
