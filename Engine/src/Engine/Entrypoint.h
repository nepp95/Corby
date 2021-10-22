#pragma once

#ifdef ENG_PLATFORM_WINDOWS

extern Engine::Application* Engine::createApplication();

int main(int argc, char** argv) {
	Engine::Log::init();

	ENG_CORE_WARN("Initialized Log!");

	auto app = Engine::createApplication();
	app->run();

	delete app;
}

#endif