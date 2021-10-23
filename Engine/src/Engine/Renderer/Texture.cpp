#include "engpch.h"
#include "Texture.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLTexture.h"

namespace Engine {
	Ref<Texture2D> Texture2D::create(const std::string& path) {
		switch (Renderer::getAPI()) {
			case RendererAPI::API::None: {
				ENG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}

			case RendererAPI::API::OpenGL: {
				return std::make_shared<OpenGLTexture2D>(path);
			}
		}

		ENG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}