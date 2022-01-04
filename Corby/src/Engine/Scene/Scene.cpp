#include "engpch.h"
#include "Scene.h"

#include "Engine/Renderer/Renderer2D.h"
#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"

#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>
#include <glm/glm.hpp>

namespace Engine
{
	static b2BodyType CorbyRigidbody2DTypeToBox2DBodyType(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static:	return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:	return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		ENG_CORE_ASSERT(false, "Unknown body type!");
		return b2_staticBody;
	}

	Scene::Scene()
	{}

	Scene::~Scene()
	{}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_registry.create(), this };

		entity.AddComponent<TransformComponent>();

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		m_physicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = m_registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = CorbyRigidbody2DTypeToBox2DBodyType(rigidbody.Type);
			bodyDef.position.Set(transform.Translation.x, transform.Translation.y);
			bodyDef.angle = transform.Rotation.z;

			b2Body* body = m_physicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rigidbody.FixedRotation);
			rigidbody.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& boxCollider = entity.GetComponent<BoxCollider2DComponent>();
				b2PolygonShape boxShape;
				boxShape.SetAsBox(boxCollider.Size.x * transform.Scale.x, boxCollider.Size.y * transform.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = boxCollider.Density;
				fixtureDef.friction = boxCollider.Friction;
				fixtureDef.restitution = boxCollider.Restitution;
				fixtureDef.restitutionThreshold = boxCollider.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		delete m_physicsWorld;
		m_physicsWorld = nullptr;
	}

	void Scene::OnUpdateRuntime(Timestep ts)
	{
		// Update scripts
		{
			m_registry.view<NativeScriptComponent>().each([=] (auto entity, auto& nsc) {
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->m_entity = Entity{ entity, this };

					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(ts);
				});
		}

		// Physics
		const int32_t velocityIterations = 6;
		const int32_t positionIterations = 2;
		m_physicsWorld->Step(ts, velocityIterations, positionIterations);

		{
			auto view = m_registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rigidbody = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*) rigidbody.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
		}

		// Render 2D
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;

		{
			auto view = m_registry.view<TransformComponent, CameraComponent>();

			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();

					break;
				}
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);

			for (auto entity : group)
			{
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int) entity);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		Renderer2D::BeginScene(camera);

		auto group = m_registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawSprite(transform.GetTransform(), sprite, (int) entity);
		}

		Renderer2D::EndScene();
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_viewportWidth = width;
		m_viewportHeight = height;

		auto view = m_registry.view<CameraComponent>();

		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity{ entity, this };
		}
		return {};
	}

	template<typename T>
	void Scene::OnComponentAdded(Entity entity, T& component)
	{
		static_assert(false);
	}

	template<>
	void Scene::OnComponentAdded<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<CameraComponent>(Entity entity, CameraComponent& component)
	{
		if (m_viewportWidth > 0 && m_viewportHeight > 0)
			component.Camera.SetViewportSize(m_viewportWidth, m_viewportHeight);
	}

	template<>
	void Scene::OnComponentAdded<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<TagComponent>(Entity entity, TagComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdded<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}
}
