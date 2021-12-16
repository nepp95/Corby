#pragma once

namespace Engine
{
	enum class FramebufferTextureFormat
	{
		None = 0,

		// Color
		RGBA8,

		// Depth/Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	struct FramebufferTextureSpecification
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format)
			: textureFormat(format)
		{}

		FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferAttachmentSpecification
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
			: attachments(attachments)
		{}

		std::vector<FramebufferTextureSpecification> attachments;
	};

	struct FramebufferSpecification
	{
		FramebufferAttachmentSpecification attachments;
		uint32_t width = 0, height = 0;
		uint32_t samples = 1;

		bool swapChainTarget = false;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const = 0;

		virtual const FramebufferSpecification& getSpecification() const = 0;

		static Ref<Framebuffer> create(const FramebufferSpecification& spec);
	};
}
