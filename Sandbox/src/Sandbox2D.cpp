#include "Sandbox2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

static const uint32_t s_mapWidth = 24;
static const char* s_mapTiles =
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWDDDDDDDDDWWWWWWWW"
"WWWWWDDDDDDDDDDDDDWWWWWW"
"WWWWWDDDDDDDDDDDDDWWWWWW"
"WWWWWDDDDDDDDDDDDDWWWWWW"
"WWWWWWWDDDDDDDDDWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWDDDDDDDDDWWWWWWWW"
"WWWWWDDDDDDDDDDDDDWWWWWW"
"WWWWWDDDDDDDDDDDDDWWWWWW"
"WWWWWDDDDDDDDDDDDDWWWWWW"
"WWWWWWWDDDDDDDDDWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
"WWWWWWWWWWWWWWWWWWWWWWWW"
;

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_cameraController(1280.0f / 720.0f), m_mapWidth(s_mapWidth), m_mapHeight(strlen(s_mapTiles) / s_mapWidth) {
}

void Sandbox2D::onAttach() {
	ENG_PROFILE_FUNCTION();

	m_cameraController.setZoomLevel(5.0f);

	m_checkerboardTexture = Engine::Texture2D::create("assets/textures/Checkerboard.png");
	m_tileset = Engine::Texture2D::create("assets/textures/tilesetkenney.png");
	m_textureMap['D'] = Engine::SubTexture2D::createFromCoords(m_tileset, { 6, 11 }, { 128, 128 });
	m_textureMap['W'] = Engine::SubTexture2D::createFromCoords(m_tileset, { 11, 11 }, { 128, 128 });
	m_textureGrass = Engine::SubTexture2D::createFromCoords(m_tileset, { 1, 11 }, { 128, 128 });
}

void Sandbox2D::onDetach() {
	ENG_PROFILE_FUNCTION();
}

void Sandbox2D::onUpdate(Engine::Timestep ts) {
	ENG_PROFILE_FUNCTION();

	// -----------------------------------------
	//
	//    Update
	//
	// -----------------------------------------
	m_cameraController.onUpdate(ts);

	// -----------------------------------------
	//
	//    Render
	//
	// -----------------------------------------
	Engine::Renderer2D::resetStats();

	{
		ENG_PROFILE_SCOPE("Renderer Prep");
		Engine::RenderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 1 });
		Engine::RenderCommand::clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		ENG_PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::beginScene(m_cameraController.getCamera());

		for (uint32_t y = 0; y < m_mapHeight; y++) {
			for (uint32_t x = 0; x < m_mapWidth; x++) {
				char tileType = s_mapTiles[x + y * m_mapWidth];
				Engine::Ref<Engine::SubTexture2D> texture;
				if (m_textureMap.find(tileType) != m_textureMap.end())
					texture = m_textureMap[tileType];
				else
					texture = m_textureGrass;

				Engine::Renderer2D::drawQuad({ x - m_mapWidth / 2.0f, y - m_mapHeight / 2.0f, 0.5f }, { 1.0f, 1.0f }, texture);
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

	ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor));

	uint32_t textureID = m_checkerboardTexture->getRendererID();
	ImGui::Image((void*)textureID, ImVec2{ 1280, 720 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

	ImGui::End();

}

void Sandbox2D::onEvent(Engine::Event& e) {
	m_cameraController.onEvent(e);
}
