#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_cameraController(1280.0f / 720.0f) {
}

void Sandbox2D::onAttach() {
	ENG_PROFILE_FUNCTION();

	m_checkerboardTexture = Engine::Texture2D::create("assets/textures/Checkerboard.png");
}

void Sandbox2D::onDetach() {
	ENG_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Engine::Timestep timestep) {
	ENG_PROFILE_FUNCTION();

	// -----------------------------------------
	//
	//    Update
	//
	// -----------------------------------------
	m_cameraController.onUpdate(timestep);

	// -----------------------------------------
	//
	//    Render
	//
	// -----------------------------------------
	Engine::Renderer2D::resetStats();

	{
		ENG_PROFILE_SCOPE("Renderer Prep");
		Engine::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::clear();
	}

	{
		static float rotation = 0.0f;
		rotation += timestep * 50.0f;

		ENG_PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::beginScene(m_cameraController.getCamera());
		Engine::Renderer2D::drawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		Engine::Renderer2D::drawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Engine::Renderer2D::drawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_squareColor);
		Engine::Renderer2D::drawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_checkerboardTexture, 10.0f);
		Engine::Renderer2D::drawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_checkerboardTexture, 20.0f);
		Engine::Renderer2D::endScene();

		Engine::Renderer2D::beginScene(m_cameraController.getCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f) {
			for (float x = -5.0f; x < 5.0f; x += 0.5f) {
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Engine::Renderer2D::drawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		Engine::Renderer2D::endScene();
	}

}

void Sandbox2D::onImGuiRender() {
	ENG_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = Engine::Renderer2D::getStats();
	ImGui::Text("Renderer2D Statistics:");
	ImGui::Text("Draw calls: %d", stats.drawCalls);
	ImGui::Text("Quads: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
	ImGui::Text("Indices: %d", stats.getTotalIndexCount());

	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));

	ImGui::End();
}

void Sandbox2D::onEvent(Engine::Event& e) {
	m_cameraController.onEvent(e);
}
