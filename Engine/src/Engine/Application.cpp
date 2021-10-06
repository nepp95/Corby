#include "engpch.h"
#include "Application.h"

#include "Log.h"
#include "Input/Input.h"

#include <glad/glad.h>

namespace Engine {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application() {
		ENGINE_CORE_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::create());
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));

		m_imGuiLayer = new ImGuiLayer();
		pushOverlay(m_imGuiLayer);
	}

	Application::~Application() {
	}

	void Application::run() {
		while (m_running) {
			// Clear screen
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			// Layers onUpdate
			for (Layer* layer : m_layerStack) {
				layer->onUpdate();
			}

			// ImGui
			m_imGuiLayer->begin();
			for (Layer* layer : m_layerStack) {
				layer->onImGuiRender();
			}
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