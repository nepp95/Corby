#include "engpch.h"
#include "Application.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/Log.h"
#include "Engine/Renderer/Renderer.h"

#include <glfw/glfw3.h>

namespace Engine
{
	Application* Application::s_instance = nullptr;

	Application::Application(const std::string& name)
	{
		ENG_PROFILE_FUNCTION();

		// Create application instance
		ENG_CORE_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;

		// Create window and bind event callback
		m_window = Scope<Window>(Window::Create(WindowProps(name)));
		m_window->SetEventCallback(ENG_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		// Create imGui layer and add it to the layerstack
		m_imGuiLayer = new ImGuiLayer();
		PushOverlay(m_imGuiLayer);
	}

	Application::~Application()
	{
		ENG_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}



	void Application::Close()
	{
		m_running = false;
	}

	void Application::OnEvent(Event& e)
	{
		ENG_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(ENG_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(ENG_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_layerStack.rbegin(); it != m_layerStack.rend(); ++it)
		{
			if (e.handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		ENG_PROFILE_FUNCTION();

		m_layerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		ENG_PROFILE_FUNCTION();

		m_layerStack.PushOverlay(layer);
	}

	void Application::Run()
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
				glfwSetWindowTitle((GLFWwindow*) m_window->GetNativeWindow(), title.c_str());

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
						layer->OnUpdate(ts);
				}
			}

			// ImGui
			m_imGuiLayer->Begin();
			{
				ENG_PROFILE_SCOPE("LayerStack::onImGuiRender");

				for (Layer* layer : m_layerStack)
					layer->OnImGuiRender();
			}
			m_imGuiLayer->End();

			m_window->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		ENG_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_minimized = true;
			return false;
		}

		m_minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}
}
