#include "engpch.h"
#include "Application.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Renderer/Renderer.h"

#include <glfw/glfw3.h>

namespace Engine
{
	Application* Application::s_instance = nullptr;

	Application::Application(const std::string& name, ApplicationCommandLineArgs args)
		: m_commandLineArgs(args)
	{
		ENG_PROFILE_FUNCTION();

		// Create application instance
		ENG_CORE_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;

		// Create window and bind event callback
		m_window = Scope<Window>(Window::create(WindowProps(name)));
		m_window->setEventCallback(ENG_BIND_EVENT_FN(Application::onEvent));

		Renderer::init();

		// Create imGui layer and add it to the layerstack
		m_imGuiLayer = new ImGuiLayer();
		pushOverlay(m_imGuiLayer);
	}

	Application::~Application()
	{
		ENG_PROFILE_FUNCTION();

		Renderer::shutdown();
	}



	void Application::close()
	{
		m_running = false;
	}

	void Application::onEvent(Event& e)
	{
		ENG_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(ENG_BIND_EVENT_FN(Application::onWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(ENG_BIND_EVENT_FN(Application::onWindowResize));

		for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
		{
			if (e.handled)
				break;
			(*it)->onEvent(e);
		}
	}

	void Application::pushLayer(Layer* layer)
	{
		ENG_PROFILE_FUNCTION();

		m_layerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* layer)
	{
		ENG_PROFILE_FUNCTION();

		m_layerStack.pushOverlay(layer);
	}

	void Application::run()
	{
		ENG_PROFILE_FUNCTION();

		// Frame counter
		float timePassed = 0.0f;
		int frames = 0;
		//

		while (m_running)
		{
			ENG_PROFILE_SCOPE("RunLoop");

			float time = (float) glfwGetTime();
			Timestep ts = time - m_lastFrameTime;
			m_lastFrameTime = time;

			// Frame counter
			timePassed += ts;
			frames++;

			if (timePassed >= 1.0f)
			{
				std::string title = std::to_string(frames) + "FPS";
				glfwSetWindowTitle((GLFWwindow*) m_window->getNativeWindow(), title.c_str());

				timePassed = 0.0f;
				frames = 0;
			}
			//

			// Layers onUpdate
			if (!m_minimized)
			{
				{
					ENG_PROFILE_SCOPE("LayerStack::onUpdate");

					for (Layer* layer : m_layerStack)
						layer->onUpdate(ts);
				}
			}

			// ImGui
			m_imGuiLayer->begin();
			{
				ENG_PROFILE_SCOPE("LayerStack::onImGuiRender");

				for (Layer* layer : m_layerStack)
					layer->onImGuiRender();
			}
			m_imGuiLayer->end();

			m_window->onUpdate();
		}
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

	bool Application::onWindowResize(WindowResizeEvent& e)
	{
		ENG_PROFILE_FUNCTION();

		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		Renderer::onWindowResize(e.getWidth(), e.getHeight());

		return false;
	}
}
