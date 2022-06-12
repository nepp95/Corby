#pragma once

#include <Engine.h>
#include "Panels/ContentBrowserPanel.h"
#include "Panels/SceneHierarchyPanel.h"

namespace Engine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		void OnOverlayRender();

		void NewScene();
		void OpenScene();
		void OpenScene(const std::filesystem::path& path);
		void SaveScene();
		void SaveSceneAs();

		void SerializeScene(Ref<Scene> scene, const std::filesystem::path& path);

		void OnScenePlay();
		void OnSceneStop();
		void OnDuplicateEntity();

		// UI Panels
		void UI_Toolbar();

	private:
		// Camera & Viewport
		CameraController m_cameraController;
		EditorCamera m_editorCamera;
		Entity m_hoveredEntity;
		int m_gizmoType = -1;
		bool m_showPhysicsColliders = false;
		bool m_primaryCamera = true;
		bool m_viewportFocused = false, m_viewportHovered = false;
		glm::vec2 m_viewportBounds[2];
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };

		// Rendering & Scene
		Ref<Framebuffer> m_framebuffer;
		Ref<Scene> m_activeScene;
		Ref<Scene> m_editorScene;
		std::filesystem::path m_editorScenePath;

		// Timesteps
		Timestep m_avgTimestep = 0.0f;
		uint8_t m_avgTimestepCounter = 0;
		Timestep m_timestep;

		enum class SceneState
		{
			Edit = 0, Play = 1
		};

		SceneState m_sceneState = SceneState::Edit;

		// Panels
		SceneHierarchyPanel m_sceneHierarchyPanel;
		ContentBrowserPanel m_contentBrowserPanel;

		// Editor resources
		std::string m_activeFile = "";
		Ref<Texture2D> m_iconPlay, m_iconStop;
	};
}
