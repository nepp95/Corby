#pragma once

#include "Engine/Renderer/Framebuffer.h"

namespace Engine {
	class OpenGLFramebuffer : public Framebuffer {
	public:
		OpenGLFramebuffer(const FramebufferSpecification& spec);
		virtual ~OpenGLFramebuffer();

		void invalidate();

		virtual void bind() override;
		virtual void unbind() override;

		virtual uint32_t getColorAttachmentRendererID() const override { return m_colorAttachment; }

		virtual const FramebufferSpecification& getSpecification() const override { return m_specification; }

	private:
		uint32_t m_rendererID;
		uint32_t m_colorAttachment, m_depthAttachment;
		FramebufferSpecification m_specification;
	};
}