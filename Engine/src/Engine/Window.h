#pragma once

#include "engpch.h"

#include "Engine/Core.h"
#include "Engine/Events/event.h"

namespace Engine {
	struct WindowProps {
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowProps(const std::string& p_title = "Game engine", unsigned int p_width = 1280, unsigned int p_height = 720) : title(p_title), width(p_width), height(p_height) {}
	};

	class ENGINE_API Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void onUpdate() = 0;

		virtual unsigned int getWidth() const = 0;
		virtual unsigned int getHeight() const = 0;

		virtual void setEventCallback(const EventCallbackFn& callback) = 0;
		virtual void setVSync(bool enabled) = 0;
		virtual bool isVSync() const = 0;

		virtual void* getNativeWindow() const = 0;

		static Window* create(const WindowProps& props = WindowProps());
	};
}