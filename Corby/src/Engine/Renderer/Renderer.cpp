#include "engpch.h"
#include "Renderer.h"

#include "Engine/Renderer/Renderer2D.h"

namespace Engine
{
	Scope<Renderer::SceneData> Renderer::m_sceneData = CreateScope<Renderer::SceneData>();

	void Renderer::Init()
	{
		ENG_PROFILE_FUNCTION();

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		m_sceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		// Bind the shader
		shader->Bind();

		// Upload uniforms
		shader->SetMat4("u_viewProjection", m_sceneData->ViewProjectionMatrix);
		shader->SetMat4("u_transform", transform);

		// Bind the vertex array and render it
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}
