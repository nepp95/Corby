#include "engpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Engine {
	RendererAPI* RenderCommand::s_rendererAPI = new OpenGLRendererAPI;

	inline void RenderCommand::setClearColor(const glm::vec4& color) {
		s_rendererAPI->setClearColor(color);
	}

	inline void RenderCommand::clear() {
		s_rendererAPI->clear();
	}

	inline void RenderCommand::drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) {
		s_rendererAPI->drawIndexed(vertexArray);
	}
}