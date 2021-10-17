#include "engpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Engine {
	unsigned int ShaderDataTypeSize(ShaderDataType type) {
		switch (type) {
			case ShaderDataType::Float:		return 4;
			case ShaderDataType::Float2:	return 4 * 2;
			case ShaderDataType::Float3:	return 4 * 3;
			case ShaderDataType::Float4:	return 4 * 4;
			case ShaderDataType::Mat3:		return 4 * 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4 * 4;
			case ShaderDataType::Int:		return 4;
			case ShaderDataType::Int2:		return 4 * 2;
			case ShaderDataType::Int3:		return 4 * 3;
			case ShaderDataType::Int4:		return 4 * 4;
			case ShaderDataType::Bool:		return 1;
		}

		ENGINE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	unsigned int BufferElement::getComponentCount() const {
		switch (type) {
			case ShaderDataType::Float:		return 1;
			case ShaderDataType::Float2:	return 2;
			case ShaderDataType::Float3:	return 3;
			case ShaderDataType::Float4:	return 4;
			case ShaderDataType::Mat3:		return 3 * 3;
			case ShaderDataType::Mat4:		return 4 * 4;
			case ShaderDataType::Int:		return 1;
			case ShaderDataType::Int2:		return 2;
			case ShaderDataType::Int3:		return 3;
			case ShaderDataType::Int4:		return 4;
			case ShaderDataType::Bool:		return 1;
		}

		ENGINE_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements) : m_elements(elements) {
		calculateOffsetsAndStride();
	}

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