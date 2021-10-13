#include "engpch.h"
#include "VertexArray.h"

#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Renderer.h"

namespace Engine {
	VertexArray* VertexArray::create() {
		switch (Renderer::getAPI()) {
			case RendererAPI::None: {
				ENGINE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}

			case RendererAPI::OpenGL: {
				return new OpenGLVertexArray();
			}
		}

		ENGINE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}