#include "FreeplayLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

Freeplay::Freeplay() : Layer("Freeplay"), m_cameraController(1280.0f / 720.0f)
{}

void Freeplay::OnAttach()
{
	ENG_PROFILE_FUNCTION();
}

void Freeplay::OnDetach()
{
	ENG_PROFILE_FUNCTION();
}

void Freeplay::OnUpdate(Engine::Timestep ts)
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
		Engine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::Clear();
	}

	{
		ENG_PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::BeginScene(m_cameraController.GetCamera());
		Engine::Renderer2D::EndScene();
	}
}

void Freeplay::OnImGuiRender()
{
	ENG_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = Engine::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Statistics:");
	ImGui::Text("Draw calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::End();
}

void Freeplay::OnEvent(Engine::Event& e)
{
	m_cameraController.OnEvent(e);
}
