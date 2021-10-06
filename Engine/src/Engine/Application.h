#pragma once

#include "Engine/Core.h"

#include "Window.h"
#include "Engine/ImGui/ImGuiLayer.h"
#include "Engine/Renderer/Shader.h"
#include "Engine/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

namespace Engine {
	class ENGINE_API Application {
	public:
		Application();
		virtual ~Application();

		void run();
		void onEvent(Event& e);
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		inline Window& getWindow() { return *m_window; }
		inline static Application& get() { return *s_instance; }

	private:
		bool onWindowClose(WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imGuiLayer;
		bool m_running = true;
		LayerStack m_layerStack;

		unsigned int m_vertexArray, m_vertexBuffer, m_indexBuffer;
		std::unique_ptr<Shader> m_shader;

		static Application* s_instance;
	};

	// To be defined in a client!
	Application* createApplication();
}