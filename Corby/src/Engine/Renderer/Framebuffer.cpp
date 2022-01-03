#include "engpch.h"
#include "Framebuffer.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace Engine
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:
			{
				ENG_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}

			case RendererAPI::API::OpenGL:
			{
				return CreateRef<OpenGLFramebuffer>(spec);
			}
		}

		ENG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
