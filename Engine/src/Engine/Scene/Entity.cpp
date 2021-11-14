#include "engpch.h"
#include "Entity.h"

namespace Engine {
	Entity::Entity(entt::entity handle, Scene* scene) : m_entityHandle(handle), m_scene(scene)
	{
	}
}