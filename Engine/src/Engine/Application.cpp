#include "Application.h"

#include "Events/ApplicationEvent.h"
#include "Log.h"

namespace Engine {
	Application::Application() {
	}

	Application::~Application() {
	}

	void Application::run() {
		WindowResizeEvent e(1280, 720);

		if (e.isInCategory(EventCategoryApplication)) {
			ENGINE_TRACE(e);
		}

		if (e.isInCategory(EventCategoryInput)) {
			ENGINE_TRACE(e);
		}

		while (true);
	}
}