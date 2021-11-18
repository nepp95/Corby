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

		m_checkerboardTexture = Texture2D::create("assets/textures/Checkerboard.png");
		m_tileset = Texture2D::create("assets/textures/tilesetkenney.png");
		m_textureMap['D'] = SubTexture2D::createFromCoords(m_tileset, { 6, 11 }, { 128, 128 });
		m_textureMap['W'] = SubTexture2D::createFromCoords(m_tileset, { 11, 11 }, { 128, 128 });
		m_textureGrass = SubTexture2D::createFromCoords(m_tileset, { 1, 11 }, { 128, 128 });

		FramebufferSpecification fbSpec;
		fbSpec.width = 1280;
		fbSpec.height = 720;
		m_framebuffer = Framebuffer::create(fbSpec);

		m_activeScene = createRef<Scene>();

		m_squareEntity = m_activeScene->createEntity("Green Square");
		m_squareEntity.addComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });

		m_cameraEntity = m_activeScene->createEntity("Camera Entity");
		m_cameraEntity.addComponent<CameraComponent>();

		m_secondCameraEntity = m_activeScene->createEntity("Clip-Space Entity");
		auto& cc = m_secondCameraEntity.addComponent<CameraComponent>();
		cc.primary = false;

		class CameraController : public ScriptableEntity {
		public:
			void onCreate() {
				auto& transform = getComponent<TransformComponent>().transform;
				transform[3][0] = rand() % 10 - 5.0f;
			}

			void onDestroy() {}

			void onUpdate(Timestep ts) {
				auto& transform = getComponent<TransformComponent>().transform;
				float speed = 5.0f;

				if (Input::isKeyPressed(Key::A))
					transform[3][0] -= speed * ts;
				if (Input::isKeyPressed(Key::D))
					transform[3][0] += speed * ts;
				if (Input::isKeyPressed(Key::W))
					transform[3][1] += speed * ts;
				if (Input::isKeyPressed(Key::S))
					transform[3][1] -= speed * ts;
			}
		};

		m_cameraEntity.addComponent<NativeScriptComponent>().bind<CameraController>();
		m_secondCameraEntity.addComponent<NativeScriptComponent>().bind<CameraController>();
	}

	void EditorLayer::onDetach() {
		ENG_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep ts) {
		ENG_PROFILE_FUNCTION();

		// -----------------------------------------
		//
		//    Update
		//
		// -----------------------------------------

		// Resize
		if (FramebufferSpecification spec = m_framebuffer->getSpecification();
			m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
			(spec.width != m_viewportSize.x || spec.height != m_viewportSize.y)) {
			m_framebuffer->resize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
			m_cameraController.onResize(m_viewportSize.x, m_viewportSize.y);
			m_activeScene->onViewportResize((uint32_t)m_viewportSize.x, (uint32_t)m_viewportSize.y);
		}

		// Camera
		if (m_viewportFocused)
			m_cameraController.onUpdate(ts);

		// -----------------------------------------
		//
		//    Render
		//
		// -----------------------------------------
		Renderer2D::resetStats();
		m_framebuffer->bind();
		RenderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 1 });
		RenderCommand::clear();

		// Update scene
		m_activeScene->onUpdate(ts);

		m_framebuffer->unbind();
	}

	void EditorLayer::onImGuiRender() {
		ENG_PROFILE_FUNCTION();

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
					Application::get().close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// -----------------------------------------
		//
		//    Settings
		//
		// -----------------------------------------
		ImGui::Begin("Settings");

		auto stats = Renderer2D::getStats();
		ImGui::Text("Renderer2D Statistics:");
		ImGui::Text("Draw calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		if (m_squareEntity) {
			ImGui::Separator();
			std::string tag = m_squareEntity.getComponent<TagComponent>();
			ImGui::Text("%s", tag.c_str());

			auto& squareColor = m_squareEntity.getComponent<SpriteRendererComponent>().color;
			ImGui::ColorEdit4("Square Color", glm::value_ptr(squareColor));
			ImGui::Separator();
		}

		ImGui::DragFloat3("Camera Transform", glm::value_ptr(m_cameraEntity.getComponent<TransformComponent>().transform[3]));

		if (ImGui::Checkbox("Camera A", &m_primaryCamera)) {
			m_cameraEntity.getComponent<CameraComponent>().primary = m_primaryCamera;
			m_secondCameraEntity.getComponent<CameraComponent>().primary = !m_primaryCamera;
		}

		{
			auto& camera = m_secondCameraEntity.getComponent<CameraComponent>().camera;
			float orthoSize = camera.getOrthographicSize();

			if (ImGui::DragFloat("Second camera ortho size", &orthoSize))
				camera.setOrthographicSize(orthoSize);
		}

		ImGui::End();

		// -----------------------------------------
		//
		//    Viewport
		//
		// -----------------------------------------
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		m_viewportFocused = ImGui::IsWindowFocused();
		m_viewportHovered = ImGui::IsWindowHovered();
		Application::get().getImGuiLayer()->blockEvents(!m_viewportFocused || !m_viewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_framebuffer->getColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void EditorLayer::onEvent(Event& e) {
		m_cameraController.onEvent(e);
	}
}
