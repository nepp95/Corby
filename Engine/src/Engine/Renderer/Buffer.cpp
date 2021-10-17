#include "engpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Engine {
	VertexBuffer* VertexBuffer::create(float* vertices, unsigned int size) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None: {
				ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}

			case RendererAPI::API::OpenGL: {
				return new OpenGLVertexBuffer(vertices, size);
			}
		}

		ENGINE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::create(unsigned int* indices, unsigned int size) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None: {
				ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}

			case RendererAPI::API::OpenGL: {
				return new OpenGLIndexBuffer(indices, size);
			}
		}

		ENGINE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void BufferLayout::calculateOffsetsAndStride() {
		unsigned int offset = 0;
		m_stride = 0;

		for (auto& element : m_elements) {
			element.offset = offset;
			offset += element.size;
			m_stride += element.size;
		}
	}
}