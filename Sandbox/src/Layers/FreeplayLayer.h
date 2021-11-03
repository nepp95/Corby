#pragma once

#include <Engine.h>

class Freeplay : public Engine::Layer {
public:
	Freeplay();
	virtual ~Freeplay() = default;

	virtual void onAttach() override;
	virtual void onDetach() override;

	void onUpdate(Engine::Timestep timestep) override;
	virtual void onImGuiRender() override;
	void onEvent(Engine::Event& e) override;

private:
	Engine::CameraController m_cameraController;
};