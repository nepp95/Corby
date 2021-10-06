#include "engpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine {
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_windowHandle(windowHandle) {
		ENGINE_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::init() {
		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		ENGINE_CORE_ASSERT(status, "Could not initialize glad!");

		ENGINE_CORE_INFO("OpenGL info:");
		ENGINE_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		ENGINE_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		ENGINE_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::swapBuffers() {
		glfwSwapBuffers(m_windowHandle);
	}
}