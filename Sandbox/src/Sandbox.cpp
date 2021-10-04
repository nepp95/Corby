#include <Engine.h>

#include "imgui/imgui.h"

class ExampleLayer : public Engine::Layer {
public:
	ExampleLayer() : Layer("Example") {
	}

	void onUpdate() override {
	}

	virtual void onImGuiRender() override {
	}

	void onEvent(Engine::Event& event) override {
	}
};

class Sandbox : public Engine::Application {
public:
	Sandbox() {
		pushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};

Engine::Application* Engine::createApplication() {
	return new Sandbox();
}