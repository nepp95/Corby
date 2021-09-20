#pragma once

#include "Engine/Core.h"
#include "Events/Event.h"

namespace Engine {
	class ENGINE_API Application {
	public:
		Application();
		virtual ~Application();

		void run();
	};

	// To be defined in a client!
	Application* createApplication();
}