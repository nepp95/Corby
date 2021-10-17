#pragma once

#include "RendererAPI.h"

namespace Engine {
	class RenderCommand {
	public:
		inline static void setClearColor(const glm::vec4& color);
		inline static void clear();
		inline static void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray);

	private:
		static RendererAPI* s_rendererAPI;
	};
}