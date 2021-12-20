#include "EditorLayer.h"

#include "Engine/Math/Math.h"
#include "Engine/Scene/SceneSerializer.h"
#include "Engine/Utils/PlatformUtils.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <ImGuizmo.h>

namespace Engine
{
	EditorLayer::EditorLayer() : Layer("EditorLayer"), m_cameraController(1280.0f / 720.0f)
	{}

	void EditorLayer::onAttach()
	{
		ENG_PROFILE_FUNCTION();

		m_cameraController.setZoomLevel(5.0f);

		FramebufferSpecification fbSpec;
		fbSpec.width = 1280;
		fbSpec.height = 720;
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		m_framebuffer = Framebuffer::create(fbSpec);

		m_activeScene = createRef<Scene>();

		auto commandLineArgs = Application::get().getCommandLineArgs();
		if (commandLineArgs.count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_activeScene);
			serializer.deserialize(sceneFilePath);
		}

		m_editorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		#if 0
		m_squareEntity = m_activeScene->createEntity("Green Square");
		m_squareEntity.addComponent<SpriteRendererComponent>(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });
		m_redSquareEntity = m_activeScene->createEntity("Red Square");
		m_redSquareEntity.addComponent<SpriteRendererComponent>(glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });

		m_cameraEntity = m_activeScene->createEntity("Camera A");
		m_cameraEntity.addComponent<CameraComponent>();

		m_secondCameraEntity = m_activeScene->createEntity("Camera B");
		auto& cc = m_secondCameraEntity.addComponent<CameraComponent>();
		cc.primary = false;

		class CameraController : public ScriptableEntity
		{
		public:
			virtual void onCreate() override
			{
				auto& translation = getComponent<TransformComponent>().translation;
				translation.x = rand() % 10 - 5.0f;
			}

			virtual void onDestroy() override {}

			virtual void onUpdate(Timestep ts) override
			{
				auto& translation = getComponent<TransformComponent>().translation;
				float speed = 5.0f;

				if (Input::isKeyPressed(Key::A))
					translation.x -= speed * ts;
				if (Input::isKeyPressed(Key::D))
					translation.x += speed * ts;
				if (Input::isKeyPressed(Key::W))
					translation.y += speed * ts;
				if (Input::isKeyPressed(Key::S))
					translation.y -= speed * ts;
			}
		};

		m_cameraEntity.addComponent<NativeScriptComponent>().bind<CameraController>();
		m_secondCameraEntity.addComponent<NativeScriptComponent>().bind<CameraController>();
		#endif

		m_sceneHierarchyPanel.setContext(m_activeScene);
	}

	void EditorLayer::onDetach()
	{
		ENG_PROFILE_FUNCTION();
	}

	void EditorLayer::onUpdate(Timestep ts)
	{
		ENG_PROFILE_FUNCTION();

		// -----------------------------------------
		//
		//    Update
		//
		// -----------------------------------------

		// Resize
		if (FramebufferSpecification spec = m_framebuffer->getSpecification();
			m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
			(spec.width != m_viewportSize.x || spec.height != m_viewportSize.y))
		{
			m_framebuffer->resize((uint32_t) m_viewportSize.x, (uint32_t) m_viewportSize.y);
			m_cameraController.onResize(m_viewportSize.x, m_viewportSize.y);
			m_editorCamera.setViewportSize(m_viewportSize.x, m_viewportSize.y);
			m_activeScene->onViewportResize((uint32_t) m_viewportSize.x, (uint32_t) m_viewportSize.y);
		}

		// Camera
		if (m_viewportFocused)
			m_cameraController.onUpdate(ts);

		m_editorCamera.onUpdate(ts);

		// -----------------------------------------
		//
		//    Render
		//
		// -----------------------------------------
		Renderer2D::resetStats();
		m_framebuffer->bind();
		RenderCommand::setClearColor({ 0.0f, 0.0f, 0.0f, 1 });
		RenderCommand::clear();

		m_framebuffer->clearAttachment(1, -1);

		// Update scene
		m_activeScene->onUpdateEditor(ts, m_editorCamera);

		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_viewportBounds[0].x;
		my -= m_viewportBounds[0].y;
		glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int) mx;
		int mouseY = (int) my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int) viewportSize.x && mouseY < (int) viewportSize.y)
		{
			int pixelData = m_framebuffer->readPixel(1, mouseX, mouseY);
			m_hoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity) pixelData, m_activeScene.get());

			// Cleanup
			m_framebuffer->unbind();
		}
	}

	void EditorLayer::onImGuiRender()
	{
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
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		style.WindowMinSize.x = minWinSizeX;

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows,
				// which we can't undo at the moment without finer window depth/z control.
				if (ImGui::MenuItem("New scene", "Ctrl+N"))
					newScene();

				if (ImGui::MenuItem("Load scene", "Ctrl+O"))
					openScene();

				if (ImGui::MenuItem("Save scene", "Ctrl+S"))
					saveScene();

				if (ImGui::MenuItem("Save scene as", "Ctrl+Shift+S"))
					saveSceneAs();

				if (ImGui::MenuItem("Exit"))
					Application::get().close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_sceneHierarchyPanel.onImGuiRender();

		// -----------------------------------------
		//
		//    Settings
		//
		// -----------------------------------------
		ImGui::Begin("Statistics");

		std::string name = "None";
		if (m_hoveredEntity)
			name = m_hoveredEntity.getComponent<TagComponent>();
		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = Renderer2D::getStats();
		ImGui::Text("Renderer2D Statistics:");
		ImGui::Text("Draw calls: %d", stats.drawCalls);
		ImGui::Text("Quads: %d", stats.quadCount);
		ImGui::Text("Vertices: %d", stats.getTotalVertexCount());
		ImGui::Text("Indices: %d", stats.getTotalIndexCount());

		ImGui::End();

		// -----------------------------------------
		//
		//    Viewport
		//
		// -----------------------------------------
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");
		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_viewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_viewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		m_viewportFocused = ImGui::IsWindowFocused();
		m_viewportHovered = ImGui::IsWindowHovered();
		Application::get().getImGuiLayer()->blockEvents(!m_viewportFocused && !m_viewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_framebuffer->getColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		// -----------------------------------------
		//
		//    ImGuizmo
		//
		// -----------------------------------------
		Entity selectedEntity = m_sceneHierarchyPanel.getSelectedEntity();
		if (selectedEntity && m_gizmoType != -1)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_viewportBounds[0].x, m_viewportBounds[0].y, m_viewportBounds[1].x - m_viewportBounds[0].x, m_viewportBounds[1].y - m_viewportBounds[0].y);

			// Camera
			//auto cameraEntity = m_activeScene->getPrimaryCameraEntity();
			//const auto& camera = cameraEntity.getComponent<CameraComponent>().camera;
			//const glm::mat4& cameraProjection = camera.getProjection();
			//glm::mat4 cameraView = glm::inverse(cameraEntity.getComponent<TransformComponent>().getTransform());
			const glm::mat4& cameraProjection = m_editorCamera.getProjection();
			glm::mat4 cameraView = m_editorCamera.getViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.getComponent<TransformComponent>();
			glm::mat4 transform = tc.getTransform();

			// Snapping
			bool snap = Input::isKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_gizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION) m_gizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::decomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.rotation;
				tc.translation = translation;
				tc.rotation += deltaRotation;
				tc.scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void EditorLayer::onEvent(Event& e)
	{
		m_cameraController.onEvent(e);
		m_editorCamera.onEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<KeyPressedEvent>(ENG_BIND_EVENT_FN(EditorLayer::onKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(ENG_BIND_EVENT_FN(EditorLayer::onMouseButtonPressed));
	}

	bool EditorLayer::onKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.getRepeatCount() > 0)
			return false;

		bool control = Input::isKeyPressed(Key::LeftControl) || Input::isKeyPressed(Key::RightControl);
		bool shift = Input::isKeyPressed(Key::LeftShift) || Input::isKeyPressed(Key::RightShift);

		switch (e.getKeyCode())
		{
			case Key::N:
			{
				if (control)
					newScene();
				break;
			}

			case Key::O:
			{
				if (control)
					openScene();
				break;
			}

			case Key::S:
			{
				if (control && shift)
					saveSceneAs();
				else if (control)
					saveScene();
				break;
			}

			//Gizmos
			case Key::Q:
			{
				if (!ImGuizmo::IsUsing())
					m_gizmoType = -1;
				break;
			}

			case Key::W:
			{
				if (!ImGuizmo::IsUsing())
					m_gizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}

			case Key::E:
			{
				if (!ImGuizmo::IsUsing())
					m_gizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}

			case Key::R:
			{
				if (!ImGuizmo::IsUsing())
					m_gizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			}
		}
	}

	bool EditorLayer::onMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.getMouseButton() == Mouse::ButtonLeft)
		{
			if (m_viewportHovered && !ImGuizmo::IsOver() && !Input::isKeyPressed(Key::LeftAlt))
				m_sceneHierarchyPanel.setSelectedEntity(m_hoveredEntity);
		}

		return false;
	}

	void EditorLayer::newScene()
	{
		m_activeScene = createRef<Scene>();
		m_activeScene->onViewportResize((uint32_t) m_viewportSize.x, (uint32_t) m_viewportSize.y);
		m_sceneHierarchyPanel.setContext(m_activeScene);
	}

	void EditorLayer::openScene()
	{
		std::string filepath = FileDialogs::openFile("Engine scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			m_activeFile = filepath;

			m_activeScene = createRef<Scene>();
			m_activeScene->onViewportResize((uint32_t) m_viewportSize.x, (uint32_t) m_viewportSize.y);
			m_sceneHierarchyPanel.setContext(m_activeScene);

			SceneSerializer serializer(m_activeScene);
			serializer.deserialize(filepath);
		}
	}

	void EditorLayer::saveScene()
	{
		if (m_activeFile != "")
		{
			SceneSerializer serializer(m_activeScene);
			serializer.serialize(m_activeFile);
		} else
		{
			saveSceneAs();
		}
	}

	void EditorLayer::saveSceneAs()
	{
		std::string filepath = FileDialogs::saveFile("Engine scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(m_activeScene);
			serializer.serialize(filepath);
		}
	}
}
