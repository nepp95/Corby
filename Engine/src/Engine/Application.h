#pragma once

#include "Engine/Core.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"
#include "Renderer/Camera.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "LayerStack.h"
#include "Window.h"

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
		OrthographicCamera m_camera;

		std::shared_ptr<Shader> m_shader;
		std::shared_ptr<VertexArray> m_vertexArray;
		std::shared_ptr<Shader> m_blueShader;
		std::shared_ptr<VertexArray> m_squareVA;

		static Application* s_instance;
	};

	// To be defined in a client!
	Application* createApplication();
}