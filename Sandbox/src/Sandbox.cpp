#include <Engine.h>
#include <Engine/Core/Entrypoint.h>

//#include "Layers/FreeplayLayer.h"
#include "Sandbox2D.h"

class Sandbox : public Engine::Application
{
public:
	Sandbox(Engine::ApplicationCommandLineArgs args)
	{
		//pushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
		//pushLayer(new Freeplay());
	}

	~Sandbox() {}
};

Engine::Application* Engine::CreateApplication(Engine::ApplicationCommandLineArgs args)
{
	return new Sandbox(args);
}
