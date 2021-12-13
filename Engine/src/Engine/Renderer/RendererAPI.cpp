#include "engpch.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Engine
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:
		{
			ENG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		}

		case RendererAPI::API::OpenGL:
		{
			return createScope<OpenGLRendererAPI>();
		}
		}

		ENG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
