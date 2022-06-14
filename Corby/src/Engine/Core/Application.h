#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/LayerStack.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Core/Window.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/Event.h"
#include "Engine/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Engine
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			ENG_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	class Application
	{
	public:
		Application(const std::string& name = "CorbyEd", ApplicationCommandLineArgs args = ApplicationCommandLineArgs());
		virtual ~Application();

		void Close();
		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		ImGuiLayer* GetImGuiLayer() { return m_imGuiLayer; }
		Window& GetWindow() { return *m_window; }
		static Application& Get() { return *s_instance; }

		ApplicationCommandLineArgs GetCommandLineArgs() const { return m_commandLineArgs; }

	private:
		void Run();
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		ApplicationCommandLineArgs m_commandLineArgs;
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
	Application* CreateApplication(ApplicationCommandLineArgs args);
}
