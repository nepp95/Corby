#pragma once

#include "Engine/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Engine
{
	class SubTexture2D
	{
	public:
		SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max);

		const Ref<Texture2D> getTexture() const { return m_texture; }
		const glm::vec2* getTexCoords() const { return m_texCoords; }

		static Ref<SubTexture2D> createFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& tileSize = { 1, 1 });

	private:
		Ref<Texture2D> m_texture;
		glm::vec2 m_texCoords[4];
	};
}
