#include "engpch.h"
#include "UniformBuffer.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Engine
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
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
				return CreateRef<OpenGLUniformBuffer>(size, binding);
			}
		}

		ENG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
