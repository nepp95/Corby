#pragma once

#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

#include <glm/glm.hpp>

namespace Engine {
	class Input {
	public:
		virtual ~Input() = default;

		static bool isKeyPressed(KeyCode key);
		static bool isMouseButtonPressed(MouseCode button);
		static glm::vec2 getMousePosition();
		static float getMouseX();
		static float getMouseY();
	};
}
