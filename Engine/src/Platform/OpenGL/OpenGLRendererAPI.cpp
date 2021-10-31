#include "engpch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Engine {
	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam) {
		switch (severity) {
			case GL_DEBUG_SEVERITY_HIGH: {
				ENG_CORE_CRITICAL(message);
				return;
			}

			case GL_DEBUG_SEVERITY_MEDIUM: {
				ENG_CORE_ERROR(message);
				return;
			}

			case GL_DEBUG_SEVERITY_LOW: {
				ENG_CORE_WARN(message);
				return;
			}

			case GL_DEBUG_SEVERITY_NOTIFICATION: {
				ENG_CORE_TRACE(message);
				return;
			}
		}
	}

	void OpenGLRendererAPI::init() {
		ENG_PROFILE_FUNCTION();

		#ifdef ENG_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
		#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::setClearColor(const glm::vec4& color) {
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::drawIndexed(const Ref<VertexArray>& vertexArray) {
		glDrawElements(GL_TRIANGLES, vertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}