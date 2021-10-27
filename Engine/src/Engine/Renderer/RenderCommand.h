#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Engine {
	class RenderCommand {
	public:
		inline static void init() {
			s_rendererAPI->init();
		}

		inline static void setClearColor(const glm::vec4& color) {
			s_rendererAPI->setClearColor(color);
		}

		inline static void clear() {
			s_rendererAPI->clear();
		}

		inline static void drawIndexed(const Ref<VertexArray>& vertexArray) {
			s_rendererAPI->drawIndexed(vertexArray);
		}

	private:
		static RendererAPI* s_rendererAPI;
	};
}