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

		void NewScene();
		void OpenScene();
		void SaveScene();
		void SaveSceneAs();

	private:
		std::string m_activeFile = "";

		CameraController m_cameraController;
		EditorCamera m_editorCamera;
		bool m_primaryCamera = true;
		bool m_viewportFocused = false, m_viewportHovered = false;
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };
		glm::vec2 m_viewportBounds[2];

		// Panels
		SceneHierarchyPanel m_sceneHierarchyPanel;
		ContentBrowserPanel m_contentBrowserPanel;
		int m_gizmoType = -1;

		// Temp
		Ref<VertexArray> m_squareVA;
		Ref<Shader> m_flatColorShader;
		Ref<Framebuffer> m_framebuffer;
		Ref<Scene> m_activeScene;

		Entity m_hoveredEntity;
	};
}
