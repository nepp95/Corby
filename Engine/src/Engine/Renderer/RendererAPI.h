#pragma once

#include "Engine/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace Engine {
	class RendererAPI {
	public:
		enum class API {
			None = 0, OpenGL = 1
		};

		// Override the functions in platform specific API (OpenGL/DirectX/Vulkan etc)
		virtual void init() = 0;
		virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void clear() = 0;
		virtual void drawIndexed(const Ref<VertexArray>& vertexArray) = 0;

		inline static API getAPI() { return s_API; }
		static Scope<RendererAPI> create();

	private:
		static API s_API;
	};
}
