#include "engpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_windowHandle(windowHandle)
	{
		ENG_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void OpenGLContext::init()
	{
		ENG_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
		ENG_CORE_ASSERT(status, "Could not initialize glad!");

		ENG_CORE_INFO("OpenGL info:");
		ENG_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		ENG_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		ENG_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		ENG_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Engine requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::swapBuffers()
	{
		ENG_PROFILE_FUNCTION();

		glfwSwapBuffers(m_windowHandle);
	}
}
