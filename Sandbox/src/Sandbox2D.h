#pragma once

#include <Engine.h>

class Sandbox2D : public Engine::Layer {
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Engine::Timestep timestep) override;
	virtual void onImGuiRender() override;
	void onEvent(Engine::Event& e) override;

private:
	Engine::CameraController m_cameraController;

	// Temp
	Engine::Ref<Engine::VertexArray> m_squareVA;
	Engine::Ref<Engine::Shader> m_flatColorShader;
	Engine::Ref<Engine::Framebuffer> m_framebuffer;

	Engine::Ref<Engine::Texture2D> m_checkerboardTexture;
	Engine::Ref<Engine::Texture2D> m_tileset;
	Engine::Ref<Engine::SubTexture2D> m_textureGrass, m_textureDirt;

	uint32_t m_mapWidth, m_mapHeight;
	std::unordered_map<char, Engine::Ref<Engine::SubTexture2D>> m_textureMap;
	//Engine::Ref<Engine::Tileset> m_tileset;
	glm::vec4 m_squareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};