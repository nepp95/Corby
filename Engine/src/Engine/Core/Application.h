#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/LayerStack.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Core/Window.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/Event.h"
#include "Engine/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Engine {
	class Application {
	public:
		Application();
		virtual ~Application();

		void onEvent(Event& e);
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		inline Window& getWindow() { return *m_window; }
		inline static Application& get() { return *s_instance; }

	private:
		void run();
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResize(WindowResizeEvent& e);

	private:
		Scope<Window> m_window;
		ImGuiLayer* m_imGuiLayer;
		bool m_running = true;
		bool m_minimized = false;
		LayerStack m_layerStack;
		float m_lastFrameTime = 0.0f;

		static Application* s_instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in a client!
	Application* createApplication();
}