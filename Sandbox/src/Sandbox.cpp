#include <Engine.h>
#include <Engine/Core/Entrypoint.h>

#include "Layers/FreeplayLayer.h"
#include "ExampleLayer.h"
#include "Sandbox2D.h"

class Sandbox : public Engine::Application {
public:
	Sandbox() {
		//pushLayer(new ExampleLayer());
		//pushLayer(new Sandbox2D());
		pushLayer(new Freeplay());
	}

	~Sandbox() {}
};

Engine::Application* Engine::createApplication() {
	return new Sandbox();
}