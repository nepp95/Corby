#include "FreeplayLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

Freeplay::Freeplay() : Layer("Freeplay"), m_cameraController(1280.0f / 720.0f) {
}

void Freeplay::onAttach() {
	ENG_PROFILE_FUNCTION();
}

void Freeplay::onDetach() {
	ENG_PROFILE_FUNCTION();
}

void Freeplay::onUpdate(Engine::Timestep timestep) {
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
		ENG_PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::beginScene(m_cameraController.getCamera());
		Engine::Renderer2D::endScene();
	}
}

void Freeplay::onImGuiRender() {
	ENG_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = Engine::Renderer2D::getStats();
	ImGui::Text("Renderer2D Statistics:");
	ImGui::Text("Draw calls: %d", stats.drawCalls);
	ImGui::Text("Quads: %d", stats.quadCount);
	ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
	ImGui::Text("Indices: %d", stats.getTotalIndexCount());

	ImGui::End();
}

void Freeplay::onEvent(Engine::Event& e) {
	m_cameraController.onEvent(e);
}
