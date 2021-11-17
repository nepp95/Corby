#pragma once

#include "Engine/Scene/Entity.h"

namespace Engine {
	class ScriptableEntity {
	public:
		template<typename T>
		T& getComponent() {
			return m_entity.getComponent<T>();
		}

	private:
		Entity m_entity;
		friend class Scene;
	};
}