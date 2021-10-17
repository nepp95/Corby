#include "engpch.h"
#include "Renderer.h"

namespace Engine {
	Renderer::SceneData* Renderer::m_sceneData = new Renderer::SceneData;

	void Renderer::beginScene(OrthographicCamera& camera) {
		m_sceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::endScene() {
	}

	void Renderer::submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray) {
		shader->bind();
		shader->uploadUniformMat4("u_viewProjection", m_sceneData->viewProjectionMatrix);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}