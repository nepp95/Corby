#include <Engine.h>
#include <Engine/Core/Entrypoint.h>

//#include "Layers/FreeplayLayer.h"
#include "Sandbox2D.h"

class Sandbox : public Engine::Application
{
public:
	Sandbox(const Engine::ApplicationSpecification& specification)
		: Engine::Application(specification)
	{
		//pushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
		//pushLayer(new Freeplay());
	}

	~Sandbox() {}
};

Engine::Application* Engine::CreateApplication(Engine::ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Sandbox";
	spec.WorkingDirectory = "../CorbyEd";
	spec.CommandLineArgs = args;

	return new Sandbox(spec);
}
