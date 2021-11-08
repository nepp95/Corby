#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Timestep.h"
#include "Engine/Events/Event.h"

namespace Engine {
	class Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void onAttach() {}
		virtual void onDetach() {}
		virtual void onUpdate(Timestep ts) {}
		virtual void onImGuiRender() {}
		virtual void onEvent(Event& e) {}

		const std::string& getName() const { return m_debugName; }

	protected:
		std::string m_debugName;
	};
}