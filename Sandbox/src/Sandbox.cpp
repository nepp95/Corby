#include <Engine.h>

class ExampleLayer : public Engine::Layer {
public:
	ExampleLayer() : Layer("Example") {

	}

	void onUpdate() override {

	}

	void onEvent(Engine::Event& event) override {
	}
};

class Sandbox : public Engine::Application {
public:
	Sandbox() {
		pushLayer(new ExampleLayer());
		pushOverlay(new Engine::ImGuiLayer());
	}
	~Sandbox() {}
};

Engine::Application* Engine::createApplication() {
	return new Sandbox();
}