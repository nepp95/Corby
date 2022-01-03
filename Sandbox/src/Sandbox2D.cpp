#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_cameraController(1280.0f / 720.0f)
{}

void Sandbox2D::OnAttach()
{
	ENG_PROFILE_FUNCTION();

	m_cameraController.SetZoomLevel(5.0f);
}

void Sandbox2D::OnDetach()
{
	ENG_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Engine::Timestep ts)
{
	ENG_PROFILE_FUNCTION();

	// -----------------------------------------
	//
	//    Update
	//
	// -----------------------------------------
	m_cameraController.OnUpdate(ts);

	// -----------------------------------------
	//
	//    Render
	//
	// -----------------------------------------
	Engine::Renderer2D::ResetStats();

	{
		ENG_PROFILE_SCOPE("Renderer Prep");
		Engine::RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1 });
		Engine::RenderCommand::Clear();
	}

	{
		ENG_PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::BeginScene(m_cameraController.GetCamera());
		Engine::Renderer2D::EndScene();
	}

}

void Sandbox2D::OnImGuiRender()
{
	ENG_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = Engine::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Statistics:");
	ImGui::Text("Draw calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor));

	uint32_t textureID = m_checkerboardTexture->GetRendererID();
	ImGui::Image((void*) textureID, ImVec2{ 1280, 720 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	ImGui::End();

}

void Sandbox2D::OnEvent(Engine::Event& e)
{
	m_cameraController.OnEvent(e);
}
