#pragma once

#include <Engine.h>
#include "Panels/SceneHierarchyPanel.h"

namespace Engine {
	class EditorLayer : public Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		void onUpdate(Timestep ts) override;
		virtual void onImGuiRender() override;
		void onEvent(Event& e) override;

	private:
		bool onKeyPressed(KeyPressedEvent& e);

		void newScene();
		void openScene();
		void saveScene();
		void saveSceneAs();

	private:
		std::string m_activeFile = "";

		CameraController m_cameraController;
		bool m_viewportFocused = false, m_viewportHovered = false;
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };

		// Panels
		SceneHierarchyPanel m_sceneHierarchyPanel;
		int m_gizmoType = -1;

		// Temp
		Ref<VertexArray> m_squareVA;
		Ref<Shader> m_flatColorShader;
		Ref<Framebuffer> m_framebuffer;
		Ref<Scene> m_activeScene;

		bool m_primaryCamera = true;
	};
}
