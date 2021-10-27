#include "engpch.h"
#include "WindowsInput.h"

#include "Engine/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Engine {
	Input* Input::s_instance = new WindowsInput();

	bool WindowsInput::isKeyPressedImpl(int keycode) {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetKey(window, keycode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::isMouseButtonPressedImpl(int button) {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		auto state = glfwGetMouseButton(window, button);

		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::getMousePositionImpl() {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::getMouseXImpl() {
		auto [x, y] = getMousePositionImpl();

		return x;
	}

	float WindowsInput::getMouseYImpl() {
		auto [x, y] = getMousePositionImpl();

		return y;
	}
}