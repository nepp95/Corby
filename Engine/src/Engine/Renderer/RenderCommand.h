#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Engine {
	class RenderCommand {
	public:
		static void init() {
			s_rendererAPI->init();
		}

		static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_rendererAPI->setViewport(x, y, width, height);
		}

		static void setClearColor(const glm::vec4& color) {
			s_rendererAPI->setClearColor(color);
		}

		static void clear() {
			s_rendererAPI->clear();
		}

		static void drawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0) {
			s_rendererAPI->drawIndexed(vertexArray, count);
		}

	private:
		static Scope<RendererAPI> s_rendererAPI;
	};
}