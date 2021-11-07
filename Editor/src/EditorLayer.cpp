#include "EditorLayer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

namespace Engine {
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

	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_cameraController(1280.0f / 720.0f), m_mapWidth(s_mapWidth), m_mapHeight(strlen(s_mapTiles) / s_mapWidth) {
	}

	void EditorLayer::onAttach() {
		ENG_PROFILE_FUNCTION();

		m_cameraController.setZoomLevel(5.0f);

		m_checkerboardTexture = Engine::Texture2D::create("assets/textures/Checkerboard.png");
		m_tileset = Engine::Texture2D::create("assets/textures/tilesetkenney.png");
		m_textureMap['D'] = Engine::SubTexture2D::createFromCoords(m_tileset, { 6, 11 }, { 128, 128 });
		m_textureMap['W'] = Engine::SubTexture2D::createFromCoords(m_tileset, { 11, 11 }, { 128, 128 });
		m_textureGrass = Engine::SubTexture2D::createFromCoords(m_tileset, { 1, 11 }, { 128, 128 });

		Engine::FramebufferSpecification fbSpec;
		fbSpec.width = 1280;
		fbSpec.height = 720;
		m_framebuffer = Engine::Framebuffer::create(fbSpec);
	}

	void EditorLayer::onDetach() {
		ENG_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Engine::Timestep timestep) {
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
			m_framebuffer->bind();
			Engine::RenderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 1 });
			Engine::RenderCommand::clear();
		}

		{
			static float rotation = 0.0f;
			rotation += timestep * 50.0f;

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
			m_framebuffer->unbind();
		}

	}

	void EditorLayer::onImGuiRender() {
		ENG_PROFILE_FUNCTION();

		// Note: Switch this to true to enable dockspace
		static bool dockingEnabled = true;

		if (dockingEnabled) {
			static bool dockspaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			bool opt_fullscreen = opt_fullscreen_persistant;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				const ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
			// and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
			// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
			// all active windows docked into it will lose their parent and become undocked.
			// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
			// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// Submit the DockSpace
			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows,
					// which we can't undo at the moment without finer window depth/z control.
					if (ImGui::MenuItem("Exit"))
						Engine::Application::get().close();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			ImGui::Begin("Settings");

			auto stats = Engine::Renderer2D::getStats();
			ImGui::Text("Renderer2D Statistics:");
			ImGui::Text("Draw calls: %d", stats.drawCalls);
			ImGui::Text("Quads: %d", stats.quadCount);
			ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
			ImGui::Text("Indices: %d", stats.getTotalIndexCount());

			ImGui::ColorEdit4("Square Color", glm::value_ptr(m_squareColor));

			uint32_t textureID = m_framebuffer->getColorAttachmentRendererID();
			ImGui::Image((void*)textureID, ImVec2{ 1280, 720 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			ImGui::End();
			ImGui::End();
		}
		else {
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
	}

	void EditorLayer::onEvent(Engine::Event& e) {
		m_cameraController.onEvent(e);
	}
}