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
		void saveSceneAs();

	private:
		CameraController m_cameraController;
		bool m_viewportFocused = false, m_viewportHovered = false;
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };

		// Panels
		SceneHierarchyPanel m_sceneHierarchyPanel;

		// Temp
		Ref<VertexArray> m_squareVA;
		Ref<Shader> m_flatColorShader;
		Ref<Framebuffer> m_framebuffer;
		Ref<Scene> m_activeScene;
		Entity m_squareEntity, m_redSquareEntity;
		Entity m_cameraEntity;
		Entity m_secondCameraEntity;

		bool m_primaryCamera = true;

		Ref<Texture2D> m_checkerboardTexture;
		Ref<Texture2D> m_tileset;
		Ref<SubTexture2D> m_textureGrass, m_textureDirt;

		uint32_t m_mapWidth, m_mapHeight;
		std::unordered_map<char, Ref<SubTexture2D>> m_textureMap;
	};
}
