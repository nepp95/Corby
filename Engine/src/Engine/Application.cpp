#include "engpch.h"
#include "Application.h"

#include "Engine/Input/Input.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Log.h"

#include <glfw/glfw3.h>

namespace Engine {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application() {
		// Create application instance
		ENG_CORE_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;

		// Create window and bind event callback
		m_window = Scope<Window>(Window::create());
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));

		Renderer::init();

		// Create imGui layer and add it to the layerstack
		m_imGuiLayer = new ImGuiLayer();
		pushOverlay(m_imGuiLayer);
	}

	Application::~Application() {
	}

	void Application::run() {
		// Frame counter
		float timePassed = 0.0f;
		int frames = 0;
		//

		while (m_running) {
			float time = (float)glfwGetTime();
			Timestep timestep = time - m_lastFrameTime;
			m_lastFrameTime = time;

			// Frame counter
			timePassed += timestep;
			frames++;

			if (timePassed >= 1.0f) {
				std::string title = std::to_string(frames) + "FPS";
				glfwSetWindowTitle((GLFWwindow*)m_window->getNativeWindow(), title.c_str());

				timePassed = 0.0f;
				frames = 0;
			}
			//

			// Layers onUpdate
			for (Layer* layer : m_layerStack)
				layer->onUpdate(timestep);

			// ImGui
			m_imGuiLayer->begin();

			for (Layer* layer : m_layerStack)
				layer->onImGuiRender();

			m_imGuiLayer->end();
			//

			m_window->onUpdate();
		}
	}

	void Application::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->onEvent(e);

			if (e.handled) {
				break;
			}
		}
	}

	void Application::pushLayer(Layer* layer) {
		m_layerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* layer) {
		m_layerStack.pushOverlay(layer);
	}

	bool Application::onWindowClose(WindowCloseEvent& e) {
		m_running = false;
		return true;
	}
}