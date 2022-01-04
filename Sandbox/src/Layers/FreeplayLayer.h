#pragma once

#include <Engine.h>

class Freeplay : public Engine::Layer
{
public:
	Freeplay();
	virtual ~Freeplay() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Engine::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Engine::Event& e) override;

private:
	Engine::CameraController m_cameraController;
};
