#pragma once

#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

#include <glm/glm.hpp>

namespace Engine
{
	class Input
	{
	public:
		virtual ~Input() = default;

		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
