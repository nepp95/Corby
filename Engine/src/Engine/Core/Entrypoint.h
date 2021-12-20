#pragma once

#include "Engine/Core/Application.h"
#include "Engine/Core/Base.h"

#ifdef ENG_PLATFORM_WINDOWS

extern Engine::Application* Engine::createApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Engine::Log::init();

	ENG_PROFILE_BEGIN_SESSION("Startup", "EngineProfile-Startup.json");
	auto app = Engine::createApplication({ argc, argv });
	ENG_PROFILE_END_SESSION();

	ENG_PROFILE_BEGIN_SESSION("Runtime", "EngineProfile-Runtime.json");
	app->run();
	ENG_PROFILE_END_SESSION();

	ENG_PROFILE_BEGIN_SESSION("Shutdown", "EngineProfile-Shutdown.json");
	delete app;
	ENG_PROFILE_END_SESSION();
}

#endif
