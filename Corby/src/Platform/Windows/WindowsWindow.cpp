#include "engpch.h"
#include "WindowsWindow.h"

#include "Engine/Core/Input.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Engine
{
	static uint8_t s_GLFWWindowCount = 0;
	static void GLFWErrorCallback(int error, const char* description)
	{
		ENG_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return CreateScope<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		ENG_PROFILE_FUNCTION();

		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		ENG_PROFILE_FUNCTION();

		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		ENG_PROFILE_FUNCTION();

		m_data.Title = props.Title;
		m_data.Width = props.Width;
		m_data.Height = props.Height;

		ENG_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (s_GLFWWindowCount == 0)
		{
			ENG_PROFILE_SCOPE("glfwInit");

			int success = glfwInit();
			ENG_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			ENG_PROFILE_SCOPE("glfwCreateWindow");

			#ifdef ENG_DEBUG
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			#endif

			m_window = glfwCreateWindow((int) props.Width, (int) props.Height, m_data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}

		m_context = GraphicsContext::Create(m_window);
		m_context->Init();

		glfwSetWindowUserPointer(m_window, &m_data);
		SetVSync(true);

		// Setup callbacks
		// Window size change callback
		glfwSetWindowSizeCallback(m_window, [] (GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
			});

		// Window close callback
		glfwSetWindowCloseCallback(m_window, [] (GLFWwindow* window) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
			});

		// Key press callback
		glfwSetKeyCallback(m_window, [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, true);
					data.EventCallback(event);
					break;
				}
			}
			});

		glfwSetCharCallback(m_window, [] (GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
			});

		// Mouse button press callback
		glfwSetMouseButtonCallback(m_window, [] (GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
			});

		// Mouse scroll callback
		glfwSetScrollCallback(m_window, [] (GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float) xOffset, (float) yOffset);
			data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_window, [] (GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *(WindowData*) glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float) xPos, (float) yPos);
			data.EventCallback(event);
			});
	}

	void WindowsWindow::Shutdown()
	{
		ENG_PROFILE_FUNCTION();

		glfwDestroyWindow(m_window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		ENG_PROFILE_FUNCTION();

		glfwPollEvents();

		m_context->SwapBuffers();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		ENG_PROFILE_FUNCTION();

		if (enabled)
		{
			glfwSwapInterval(1);
		} else
		{
			glfwSwapInterval(0);
		}

		m_data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_data.VSync;
	}
}
