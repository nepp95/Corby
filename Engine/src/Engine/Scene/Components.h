#pragma once

#include "Engine/Scene/SceneCamera.h"
#include "Engine/Scene/ScriptableEntity.h"

#include <glm/glm.hpp>

namespace Engine {
	struct TagComponent {
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: tag(tag) {}

		operator std::string& () { return tag; }
		operator const std::string& () const { return tag; }
	};

	struct TransformComponent {
		glm::mat4 transform{ 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::mat4& transform)
			: transform(transform) {}

		operator glm::mat4& () { return transform; }
		operator const glm::mat4& () const { return transform; }
	};

	struct SpriteRendererComponent {
		glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: color(color) {}
	};

	struct CameraComponent {
		SceneCamera camera;
		bool primary = true;
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct NativeScriptComponent {
		ScriptableEntity* instance = nullptr;

		std::function<void()> instantiateFunction;
		std::function<void()> destroyInstanceFunction;

		std::function<void(ScriptableEntity*)> onCreateFunction;
		std::function<void(ScriptableEntity*)> onDestroyFunction;
		std::function<void(ScriptableEntity*, Timestep)> onUpdateFunction;

		template<typename T>
		void bind() {
			instantiateFunction = [&]() { instance = new T(); };
			destroyInstanceFunction = [&]() {
				delete (T*)instance;
				instance = nullptr;
			};

			onCreateFunction = [](ScriptableEntity* instance) { ((T*)instance)->onCreate(); };
			onDestroyFunction = [](ScriptableEntity* instance) { ((T*)instance)->onDestroy(); };
			onUpdateFunction = [](ScriptableEntity* instance, Timestep ts) { ((T*)instance)->onUpdate(ts); };
		}
	};
}