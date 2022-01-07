#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Core/UUID.h"
#include "Engine/Renderer/EditorCamera.h"

#include <entt.hpp>

class b2World;

namespace Engine
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		void DestroyEntity(Entity entity);
		void DuplicateEntity(Entity entity);

		template<typename Component>
		static void CopyComponent(entt::registry& dst, entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap);
		template<typename Component>
		static void CopyComponentIfExists(Entity dst, Entity src);
		static Ref<Scene> Copy(Ref<Scene> scene);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
		void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_registry.view<Components...>();
		}
	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		entt::registry m_registry;
		uint32_t m_viewportWidth = 0, m_viewportHeight = 0;

		b2World* m_physicsWorld = nullptr;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}
