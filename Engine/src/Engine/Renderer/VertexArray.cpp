#include "engpch.h"
#include "VertexArray.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Engine {
	VertexArray* VertexArray::create() {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None: {
				ENG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}

			case RendererAPI::API::OpenGL: {
				return new OpenGLVertexArray();
			}
		}

		ENG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}