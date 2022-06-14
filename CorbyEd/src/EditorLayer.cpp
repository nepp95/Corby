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
	extern const std::filesystem::path g_assetPath;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_cameraController(1280.0f / 720.0f)
	{}

	void EditorLayer::OnAttach()
	{
		ENG_PROFILE_FUNCTION();

		m_iconPlay = Texture2D::Create("Resources/Icons/PlayButton.png");
		m_iconSimulate = Texture2D::Create("Resources/Icons/SimulateButton.png");
		m_iconStop = Texture2D::Create("Resources/Icons/StopButton.png");

		FramebufferSpecification fbSpec;
		fbSpec.Width = 1280;
		fbSpec.Height = 720;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		m_framebuffer = Framebuffer::Create(fbSpec);

		m_editorScene = CreateRef<Scene>();
		m_activeScene = m_editorScene;

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(m_activeScene);
			serializer.Deserialize(sceneFilePath);
		}

		m_editorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		m_sceneHierarchyPanel.SetContext(m_activeScene);
	}

	void EditorLayer::OnDetach()
	{
		ENG_PROFILE_FUNCTION();
	}

	void EditorLayer::OnUpdate(Timestep ts)
	{
		ENG_PROFILE_FUNCTION();

		// -----------------------------------------
		//
		//    Update
		//
		// -----------------------------------------
		if (m_avgTimestep > 1.0f)
		{
			m_timestep = m_avgTimestep / m_avgTimestepCounter;
			m_avgTimestepCounter = 0;
			m_avgTimestep = 0.0f;
		} else
		{
			m_avgTimestep += ts;
			m_avgTimestepCounter++;

		}

		// Resize
		if (FramebufferSpecification spec = m_framebuffer->GetSpecification();
			m_viewportSize.x > 0.0f && m_viewportSize.y > 0.0f &&
			(spec.Width != m_viewportSize.x || spec.Height != m_viewportSize.y))
		{
			m_framebuffer->Resize((uint32_t) m_viewportSize.x, (uint32_t) m_viewportSize.y);
			m_cameraController.OnResize(m_viewportSize.x, m_viewportSize.y);
			m_editorCamera.SetViewportSize(m_viewportSize.x, m_viewportSize.y);
			m_activeScene->OnViewportResize((uint32_t) m_viewportSize.x, (uint32_t) m_viewportSize.y);
		}

		// -----------------------------------------
		//
		//    Render
		//
		// -----------------------------------------
		Renderer2D::ResetStats();
		m_framebuffer->Bind();
		RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1 });
		RenderCommand::Clear();

		m_framebuffer->ClearAttachment(1, -1);

		switch (m_sceneState)
		{
			case SceneState::Edit:
			{
				if (m_viewportFocused)
					m_cameraController.OnUpdate(ts);

				m_editorCamera.OnUpdate(ts);
				m_activeScene->OnUpdateEditor(ts, m_editorCamera);
				break;
			}

			case SceneState::Play:
			{
				m_activeScene->OnUpdateRuntime(ts);
				break;
			}

			case SceneState::Simulate:
			{
				m_editorCamera.OnUpdate(ts);
				m_activeScene->OnUpdateSimulate(ts, m_editorCamera);
				break;
			}
		}

		// Mouse picking
		auto [mx, my] = ImGui::GetMousePos();
		mx -= m_viewportBounds[0].x;
		my -= m_viewportBounds[0].y;
		glm::vec2 viewportSize = m_viewportBounds[1] - m_viewportBounds[0];
		my = viewportSize.y - my;
		int mouseX = (int) mx;
		int mouseY = (int) my;

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int) viewportSize.x && mouseY < (int) viewportSize.y)
		{
			int pixelData = m_framebuffer->ReadPixel(1, mouseX, mouseY);
			m_hoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity) pixelData, m_activeScene.get());
		}

		// Render overlays
		OnOverlayRender();

		m_framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender()
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
					NewScene();

				if (ImGui::MenuItem("Load scene", "Ctrl+O"))
					OpenScene();

				if (ImGui::MenuItem("Save scene", "Ctrl+S"))
					SaveScene();

				if (ImGui::MenuItem("Save scene as", "Ctrl+Shift+S"))
					SaveSceneAs();

				if (ImGui::MenuItem("Exit"))
					Application::Get().Close();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		m_sceneHierarchyPanel.OnImGuiRender();
		m_contentBrowserPanel.OnImGuiRender();

		// -----------------------------------------
		//
		//    Statistics
		//
		// -----------------------------------------
		ImGui::Begin("Statistics");

		std::string name = "None";
		if (m_hoveredEntity)
			name = m_hoveredEntity.GetComponent<TagComponent>();
		ImGui::Text("Hovered Entity: %s", name.c_str());

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Statistics:");
		ImGui::Text("Draw calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
		ImGui::Text("Avg. Frametime: %.2fms\n", m_timestep.GetMilliseconds());

		ImGui::End();

		// -----------------------------------------
		//
		//    Settings
		//
		// -----------------------------------------
		ImGui::Begin("Settings");
		ImGui::Checkbox("Show physics colliders", &m_showPhysicsColliders);
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
		Application::Get().GetImGuiLayer()->BlockEvents(!m_viewportFocused && !m_viewportHovered);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t textureID = m_framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_viewportSize.x, m_viewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*) payload->Data;
				OpenScene(std::filesystem::path(g_assetPath) / path);
			}
			ImGui::EndDragDropTarget();
		}

		// -----------------------------------------
		//
		//    ImGuizmo
		//
		// -----------------------------------------
		Entity selectedEntity = m_sceneHierarchyPanel.GetSelectedEntity();
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
			const glm::mat4& cameraProjection = m_editorCamera.GetProjection();
			glm::mat4 cameraView = m_editorCamera.GetViewMatrix();

			// Entity transform
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
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

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Translation = translation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		UI_Toolbar();

		ImGui::End();
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_cameraController.OnEvent(e);
		m_editorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(ENG_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseButtonPressedEvent>(ENG_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (control)
					NewScene();
				break;
			}

			case Key::O:
			{
				if (control)
					OpenScene();
				break;
			}

			case Key::S:
			{
				if (control)
				{
					if (shift)
						SaveSceneAs();
					else
						SaveScene();
				}
				break;
			}

			case Key::D:
			{
				if (control)
					OnDuplicateEntity();
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

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft)
		{
			if (m_viewportHovered && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
				m_sceneHierarchyPanel.SetSelectedEntity(m_hoveredEntity);
		}

		return false;
	}

	void EditorLayer::OnOverlayRender()
	{
		if (m_sceneState == SceneState::Play)
		{
			Entity camera = m_activeScene->GetPrimaryCameraEntity();
			if (!camera)
				return;
			Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().Camera, camera.GetComponent<TransformComponent>().GetTransform());
		} else
		{
			Renderer2D::BeginScene(m_editorCamera);
		}

		if (m_showPhysicsColliders)
		{
			// Box colliders
			{
				auto view = m_activeScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, boxCollider] = view.get<TransformComponent, BoxCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(boxCollider.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(boxCollider.Size * 2.0f, 1.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
				}
			}

			// Circle colliders
			{
				auto view = m_activeScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
				for (auto entity : view)
				{
					auto [tc, circleCollider] = view.get<TransformComponent, CircleCollider2DComponent>(entity);

					glm::vec3 translation = tc.Translation + glm::vec3(circleCollider.Offset, 0.001f);
					glm::vec3 scale = tc.Scale * glm::vec3(circleCollider.Radius * 2.0f);

					glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation)
						* glm::scale(glm::mat4(1.0f), scale);

					Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
				}
			}
		}

		Renderer2D::EndScene();
	}

	void EditorLayer::NewScene()
	{
		m_activeScene = CreateRef<Scene>();
		m_activeScene->OnViewportResize((uint32_t) m_viewportSize.x, (uint32_t) m_viewportSize.y);
		m_sceneHierarchyPanel.SetContext(m_activeScene);

		m_editorScenePath = std::filesystem::path();
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Engine Scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			m_activeFile = filepath;
			OpenScene(filepath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_sceneState != SceneState::Edit)
			OnSceneStop();

		if (path.extension().string() != ".scene")
		{
			ENG_WARN("Could not load {0} - not a scene file", path.filename().string());
			return;
		}

		Ref<Scene> newScene = CreateRef<Scene>();
		SceneSerializer serializer(newScene);
		if (serializer.Deserialize(path.string()))
		{
			m_editorScene = newScene;
			m_editorScene->OnViewportResize((uint32_t) m_viewportSize.x, (uint32_t) m_viewportSize.y);
			m_sceneHierarchyPanel.SetContext(m_editorScene);

			m_activeScene = m_editorScene;
			m_editorScenePath = path;
		}

	}

	void EditorLayer::SaveScene()
	{
		if (!m_editorScenePath.empty())
			SerializeScene(m_activeScene, m_editorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Engine scene (*.scene)\0*.scene\0");
		if (!filepath.empty())
		{
			SerializeScene(m_activeScene, filepath);
			m_editorScenePath = filepath;
		}
	}

	void EditorLayer::SerializeScene(Ref<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		if (m_sceneState == SceneState::Simulate)
			OnSceneStop();

		m_sceneState = SceneState::Play;

		m_activeScene = Scene::Copy(m_editorScene);
		m_activeScene->OnRuntimeStart();

		m_sceneHierarchyPanel.SetContext(m_activeScene);
	}

	void EditorLayer::OnSceneSimulate()
	{
		if (m_sceneState == SceneState::Play)
			OnSceneStop();

		m_sceneState = SceneState::Simulate;

		m_activeScene = Scene::Copy(m_editorScene);
		m_activeScene->OnSimulateStart();

		m_sceneHierarchyPanel.SetContext(m_activeScene);
	}

	void EditorLayer::OnSceneStop()
	{
		ENG_CORE_ASSERT(m_sceneState == SceneState::Simulate || m_sceneState == SceneState::Play);

		if (m_sceneState == SceneState::Simulate)
			m_activeScene->OnSimulateStop();
		else if (m_sceneState == SceneState::Play)
			m_activeScene->OnRuntimeStop();
		
		m_sceneState = SceneState::Edit;

		m_activeScene = m_editorScene;

		m_sceneHierarchyPanel.SetContext(m_activeScene);
	}

	void EditorLayer::OnDuplicateEntity()
	{
		if (m_sceneState != SceneState::Edit)
			return;

		Entity selectedEntity = m_sceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity)
			m_editorScene->DuplicateEntity(selectedEntity);
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		bool toolbarEnabled = (bool)m_activeScene;

		ImVec4 tintColor = ImVec4(1, 1, 1, 1);
		if (!toolbarEnabled)
			tintColor.w = 0.5f;

		float size = ImGui::GetWindowHeight() - 4.0f;
		{
			Ref<Texture2D> icon = (m_sceneState == SceneState::Edit || m_sceneState == SceneState::Simulate) ? m_iconPlay : m_iconStop;
			ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), tintColor) && toolbarEnabled)
			{
				if (m_sceneState == SceneState::Edit || m_sceneState == SceneState::Simulate)
					OnScenePlay();
				else if (m_sceneState == SceneState::Play)
					OnSceneStop();
			}
		}
		ImGui::SameLine();
		{
			Ref<Texture2D> icon = (m_sceneState == SceneState::Edit || m_sceneState == SceneState::Play) ? m_iconSimulate : m_iconStop;

			if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0, 0, 0, 0), tintColor) && toolbarEnabled)
			{
				if (m_sceneState == SceneState::Edit || m_sceneState == SceneState::Play)
					OnSceneSimulate();
				else if (m_sceneState == SceneState::Simulate)
					OnSceneStop();
			}
		}

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();
	}
}
