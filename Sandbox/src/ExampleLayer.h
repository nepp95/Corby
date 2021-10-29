#pragma once

#include <Engine.h>

class ExampleLayer : public Engine::Layer {
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Engine::Timestep timestep) override;
	virtual void onImGuiRender() override;
	void onEvent(Engine::Event& event) override;

private:
	Engine::CameraController m_cameraController;

	Engine::ShaderLibrary m_shaderLibrary;
	Engine::Ref<Engine::Shader> m_shader, m_flatColorShader;
	Engine::Ref<Engine::VertexArray> m_vertexArray, m_squareVA;
	Engine::Ref<Engine::Texture2D> m_texture, m_logo;

	glm::vec3 m_squareColor = { 0.2f, 0.3f, 0.8f };
};