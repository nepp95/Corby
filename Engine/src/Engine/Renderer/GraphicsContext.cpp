#include "engpch.h"
#include "GraphicsContext.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Engine
{
	Scope<GraphicsContext> GraphicsContext::create(void* window)
	{
		switch (Renderer::getAPI())
		{
		case RendererAPI::API::None:
		{
			ENG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		}

		case RendererAPI::API::OpenGL:
		{
			return createScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}
		}

		ENG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
