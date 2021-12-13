#include "engpch.h"
#include "Renderer.h"

#include "Engine/Renderer/Renderer2D.h"

namespace Engine
{
	Scope<Renderer::SceneData> Renderer::m_sceneData = createScope<Renderer::SceneData>();

	void Renderer::init()
	{
		ENG_PROFILE_FUNCTION();

		RenderCommand::init();
		Renderer2D::init();
	}

	void Renderer::shutdown()
	{
		Renderer2D::shutdown();
	}

	void Renderer::onWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::setViewport(0, 0, width, height);
	}

	void Renderer::beginScene(OrthographicCamera& camera)
	{
		m_sceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::endScene()
	{}

	void Renderer::submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		// Bind the shader
		shader->bind();

		// Upload uniforms
		shader->setMat4("u_viewProjection", m_sceneData->viewProjectionMatrix);
		shader->setMat4("u_transform", transform);

		// Bind the vertex array and render it
		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}
