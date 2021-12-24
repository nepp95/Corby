#include "engpch.h"
#include "Buffer.h"

#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Engine
{
	BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements)
		: m_elements(elements)
	{
		CalculateOffsetsAndStride();
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
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
				return CreateRef<OpenGLVertexBuffer>(size);
			}
		}

		ENG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, unsigned int size)
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
				return CreateRef<OpenGLVertexBuffer>(vertices, size);
			}
		}

		ENG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(unsigned int* indices, unsigned int count)
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
				return CreateRef<OpenGLIndexBuffer>(indices, count);
			}
		}

		ENG_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void BufferLayout::CalculateOffsetsAndStride()
	{
		size_t offset = 0;
		m_stride = 0;

		for (auto& element : m_elements)
		{
			element.Offset = offset;
			offset += element.Size;
			m_stride += element.Size;
		}
	}
}
