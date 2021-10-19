#include "engpch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Engine {
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type) {
			case Engine::ShaderDataType::Float:		return GL_FLOAT;
			case Engine::ShaderDataType::Float2:	return GL_FLOAT;
			case Engine::ShaderDataType::Float3:	return GL_FLOAT;
			case Engine::ShaderDataType::Float4:	return GL_FLOAT;
			case Engine::ShaderDataType::Mat3:		return GL_FLOAT;
			case Engine::ShaderDataType::Mat4:		return GL_FLOAT;
			case Engine::ShaderDataType::Int:		return GL_INT;
			case Engine::ShaderDataType::Int2:		return GL_INT;
			case Engine::ShaderDataType::Int3:		return GL_INT;
			case Engine::ShaderDataType::Int4:		return GL_INT;
			case Engine::ShaderDataType::Bool:		return GL_BOOL;
		}

		ENG_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray() {
		glCreateVertexArrays(1, &m_rendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray() {
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::bind() const {
		glBindVertexArray(m_rendererID);
	}

	void OpenGLVertexArray::unbind() const {
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) {
		ENG_CORE_ASSERT(vertexBuffer->getLayout().getElements().size(), "VertexBuffer has no layout!");

		glBindVertexArray(m_rendererID);
		vertexBuffer->bind();

		unsigned int index = 0;
		const auto& layout = vertexBuffer->getLayout();

		for (const auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.getComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.getStride(),
				(const void*)element.offset);

			index++;
		}

		m_vertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) {
		glBindVertexArray(m_rendererID);
		indexBuffer->bind();

		m_indexBuffer = indexBuffer;
	}
}