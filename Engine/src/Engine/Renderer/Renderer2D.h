#pragma once

#include "Engine/Renderer/Camera.h"
#include "Engine/Renderer/Texture.h"

namespace Engine {
	class Renderer2D {
	public:
		static void init();
		static void shutdown();

		static void beginScene(const OrthographicCamera& camera);
		static void endScene();

		static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture);
		static void drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);
	};
}