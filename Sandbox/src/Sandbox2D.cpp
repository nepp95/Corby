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
	{
		ENG_PROFILE_SCOPE("Renderer Prep");
		Engine::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::clear();
	}

	{
		ENG_PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::beginScene(m_cameraController.getCamera());
		Engine::Renderer2D::drawRotatedQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, glm::radians(-45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
		Engine::Renderer2D::drawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Engine::Renderer2D::drawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_checkerboardTexture, 10.0f);
		Engine::Renderer2D::endScene();
	}

}

void Sandbox2D::onImGuiRender() {
	ENG_PROFILE_FUNCTION();

	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));
	ImGui::End();
}

void Sandbox2D::onEvent(Engine::Event& e) {
	m_cameraController.onEvent(e);
}
