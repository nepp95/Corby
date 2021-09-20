#pragma once

#include "Event.h"

namespace Engine {
	class ENGINE_API KeyEvent : public Event {
	public:
		inline int getKeyCode() const { return m_keyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);

	protected:
		KeyEvent(int keycode) : m_keyCode(keycode) {}

		int m_keyCode;
	};

	class ENGINE_API KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), m_repeatCount(repeatCount) {}

		inline int getRepeatCount() const { return m_repeatCount; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keyCode << " (" << m_repeatCount << " repeats)";

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed);
	};

	class ENGINE_API KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keyCode;

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased);
	};
}