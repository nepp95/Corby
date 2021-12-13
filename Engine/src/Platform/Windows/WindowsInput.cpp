#include "engpch.h"

#include "Engine/Core/Application.h"
#include "Engine/Core/Input.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	bool Input::isKeyPressed(const KeyCode key)
	{
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressed(const MouseCode button)
	{
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));

		return state == GLFW_PRESS;
	}

	glm::vec2 Input::getMousePosition()
	{
		auto* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float) xpos, (float) ypos };
	}

	float Input::getMouseX()
	{
		return getMousePosition().x;
	}

	float Input::getMouseY()
	{
		return getMousePosition().y;
	}
}
