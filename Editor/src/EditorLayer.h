#pragma once

#include <Engine.h>

namespace Engine {
	class EditorLayer : public Layer {
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void onAttach() override;
		virtual void onDetach() override;

		void onUpdate(Timestep timestep) override;
		virtual void onImGuiRender() override;
		void onEvent(Event& e) override;

	private:
		CameraController m_cameraController;
		bool m_viewportFocused = false, m_viewportHovered = false;
		glm::vec2 m_viewportSize = { 0.0f, 0.0f };

		// Temp
		Ref<VertexArray> m_squareVA;
		Ref<Shader> m_flatColorShader;
		Ref<Framebuffer> m_framebuffer;

		Ref<Texture2D> m_checkerboardTexture;
		Ref<Texture2D> m_tileset;
		Ref<SubTexture2D> m_textureGrass, m_textureDirt;

		uint32_t m_mapWidth, m_mapHeight;
		std::unordered_map<char, Ref<SubTexture2D>> m_textureMap;
		//Ref<Tileset> m_tileset;
		glm::vec4 m_squareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	};
}