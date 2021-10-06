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
		//pushOverlay(m_imGuiLayer);

		// Vertex array
		glGenVertexArrays(1, &m_vertexArray);
		glBindVertexArray(m_vertexArray);

		// Vertex buffer
		glGenBuffers(1, &m_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		// Index buffer
		glGenBuffers(1, &m_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

		unsigned int indices[3] = {
			0, 1, 2
		};

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	}

	Application::~Application() {
	}

	void Application::run() {
		while (m_running) {
			// Clear screen
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindVertexArray(m_vertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

			// Layers onUpdate
			for (Layer* layer : m_layerStack) {
				layer->onUpdate();
			}

			// ImGui
			//m_imGuiLayer->begin();
			//for (Layer* layer : m_layerStack) {
			//	layer->onImGuiRender();
			//}
			//m_imGuiLayer->end();
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