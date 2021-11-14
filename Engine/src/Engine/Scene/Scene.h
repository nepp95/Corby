#pragma once

#include "Engine/Core/Timestep.h"

#include <entt.hpp>

namespace Engine {
	class Entity;

	class Scene {
	public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name = std::string());

		void onUpdate(Timestep ts);

	private:
		entt::registry m_registry;

		friend class Entity;
	};
}