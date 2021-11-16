#include "engpch.h"
#include "Scene.h"

#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"

#include <glm/glm.hpp>

namespace Engine {
	static void doMath(const glm::mat4& transform) {

	}

	static void onTransformConstruct(entt::registry& registry, entt::entity entity) {

	}

	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity = { m_registry.create(), this };

		entity.addComponent<TransformComponent>();

		auto& tag = entity.addComponent<TagComponent>();
		tag.tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::onUpdate(Timestep ts)
	{
		Camera* mainCamera = nullptr;
		glm::mat4* cameraTransform = nullptr;

		{
			auto view = m_registry.view<TransformComponent, CameraComponent>();

			for (auto entity : view) {
				auto& [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.primary) {
					mainCamera = &camera.camera;
					cameraTransform = &transform.transform;

					break;
				}
			}
		}

		if (mainCamera) {
			Renderer2D::beginScene(mainCamera->getProjection(), *cameraTransform);

			auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			for (auto entity : group) {
				auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::drawQuad(transform, sprite.color);
			}

			Renderer2D::endScene();
		}
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height)
	{
		m_viewportWidth = width;
		m_viewportHeight = height;

		auto view = m_registry.view<CameraComponent>();

		for (auto entity : view) {
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (!cameraComponent.fixedAspectRatio)
				cameraComponent.camera.setViewportSize(width, height);
		}
	}
}