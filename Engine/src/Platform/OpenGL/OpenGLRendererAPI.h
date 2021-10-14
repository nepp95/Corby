#pragma once

#include "Engine/Renderer/RendererAPI.h"

namespace Engine {
	class OpenGLRendererAPI : public RendererAPI {
	public:
		virtual void setClearColor(const glm::vec4& color) override;
		virtual void clear() override;
		virtual void drawIndexed(const std::shared_ptr<VertexArray>& vertexArray) override;
	};
}