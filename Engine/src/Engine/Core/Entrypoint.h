#pragma once

#include "Engine/Core/Core.h"

#ifdef ENG_PLATFORM_WINDOWS

extern Engine::Application* Engine::createApplication();

int main(int argc, char** argv) {
	Engine::Log::init();

	ENG_PROFILE_BEGIN_SESSION("Startup", "EngineProfile-Startup.json");
	auto app = Engine::createApplication();
	ENG_PROFILE_END_SESSION();

	ENG_PROFILE_BEGIN_SESSION("Runtime", "EngineProfile-Runtime.json");
	app->run();
	ENG_PROFILE_END_SESSION();

	ENG_PROFILE_BEGIN_SESSION("Shutdown", "EngineProfile-Shutdown.json");
	delete app;
	ENG_PROFILE_END_SESSION();
}

#endif