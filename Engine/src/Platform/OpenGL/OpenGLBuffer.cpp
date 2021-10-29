#include "engpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>

namespace Engine {
	OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices, unsigned int size) {
		ENG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer() {
		ENG_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_rendererID);
	}

	void OpenGLVertexBuffer::bind() const {
		ENG_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
	}

	void OpenGLVertexBuffer::unbind() const {
		ENG_PROFILE_FUNCTION();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, unsigned int count) : m_count(count) {
		ENG_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer() {
		ENG_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_rendererID);
	}

	void OpenGLIndexBuffer::bind() const {
		ENG_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	}

	void OpenGLIndexBuffer::unbind() const {
		ENG_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}