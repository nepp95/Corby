#include "engpch.h"
#include "SceneSerializer.h"

#include "Engine/Scene/Components.h"
#include "Engine/Scene/Entity.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();

			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();

			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);

			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();

			return true;
		}
	};
}

namespace Engine
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;

		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;

		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;

		return out;
	}

	static std::string Rigidbody2DBodyTypeToString(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static:	return "Static";
			case Rigidbody2DComponent::BodyType::Dynamic:	return "Dynamic";
			case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
		}

		ENG_CORE_ASSERT(false, "Unknown body type!");
		return {};
	}

	static Rigidbody2DComponent::BodyType Rigidbody2DBodyTypeFromString(const std::string& bodyTypeString)
	{
		if (bodyTypeString == "Static") return Rigidbody2DComponent::BodyType::Static;
		if (bodyTypeString == "Dynamic") return Rigidbody2DComponent::BodyType::Dynamic;
		if (bodyTypeString == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

		ENG_CORE_ASSERT(false, "Unkown body type!");
		return Rigidbody2DComponent::BodyType::Static;
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: m_scene(scene)
	{}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		ENG_CORE_ASSERT(entity.HasComponent<IDComponent>());

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			std::string& tag = entity.GetComponent<TagComponent>();
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;

			out << YAML::Key << "ProjectionType" << YAML::Value << (int) camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();

			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.Color;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;

			auto& circleRendererComponent = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.Color;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.Fade;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap;

			auto& rigidBody = entity.GetComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << Rigidbody2DBodyTypeToString(rigidBody.Type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rigidBody.FixedRotation;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;

			auto& boxCollider = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << boxCollider.Offset;
			out << YAML::Key << "Size" << YAML::Value << boxCollider.Size;
			out << YAML::Key << "Density" << YAML::Value << boxCollider.Density;
			out << YAML::Key << "Friction" << YAML::Value << boxCollider.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << boxCollider.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << boxCollider.RestitutionThreshold;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;

			auto& circleCollider = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << circleCollider.Offset;
			out << YAML::Key << "Radius" << YAML::Value << circleCollider.Radius;
			out << YAML::Key << "Density" << YAML::Value << circleCollider.Density;
			out << YAML::Key << "Friction" << YAML::Value << circleCollider.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << circleCollider.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << circleCollider.RestitutionThreshold;

			out << YAML::EndMap;
		}

		ENG_CORE_TRACE("Serialized entity with name = {0}", entity.GetComponent<TagComponent>().Tag);

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled"; // TODO: Add scene name
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_scene->m_registry.each([&] (auto entityID) {
			Entity entity = { entityID, m_scene.get() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{
		// Not implemented
		ENG_CORE_ASSERT(false);
	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		} catch (YAML::ParserException e)
		{
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		ENG_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();

				ENG_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

				Entity deserializedEntity = m_scene->CreateEntity(name);

				auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& cameraProperties = cameraComponent["Camera"];

					cc.Camera.SetProjectionType((SceneCamera::ProjectionType) cameraProperties["ProjectionType"].as<int>());
					cc.Camera.SetPerspectiveVerticalFOV(cameraProperties["PerspectiveFOV"].as<float>());
					cc.Camera.SetPerspectiveNearClip(cameraProperties["PerspectiveNear"].as<float>());
					cc.Camera.SetPerspectiveFarClip(cameraProperties["PerspectiveFar"].as<float>());
					cc.Camera.SetOrthographicSize(cameraProperties["OrthographicSize"].as<float>());
					cc.Camera.SetOrthographicNearClip(cameraProperties["OrthographicNear"].as<float>());
					cc.Camera.SetOrthographicFarClip(cameraProperties["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
				}

				auto circleRendererComponent = entity["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
					crc.Color = circleRendererComponent["Color"].as<glm::vec4>();
					crc.Thickness = circleRendererComponent["Thickness"].as<float>();
					crc.Fade = circleRendererComponent["Fade"].as<float>();
				}

				auto rigidbody2DComponent = entity["Rigidbody2DComponent"];
				if (rigidbody2DComponent)
				{
					auto& rigidbody = deserializedEntity.AddComponent<Rigidbody2DComponent>();
					rigidbody.Type = Rigidbody2DBodyTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
					rigidbody.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
				}

				auto boxCollider2DComponent = entity["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& boxCollider = deserializedEntity.AddComponent<BoxCollider2DComponent>();
					boxCollider.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					boxCollider.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					boxCollider.Density = boxCollider2DComponent["Density"].as<float>();
					boxCollider.Friction = boxCollider2DComponent["Friction"].as<float>();
					boxCollider.Restitution = boxCollider2DComponent["Restitution"].as<float>();
					boxCollider.RestitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
				}

				auto circleCollider2DComponent = entity["CircleCollider2DComponent"];
				if (circleCollider2DComponent)
				{
					auto& circleCollider = deserializedEntity.AddComponent<CircleCollider2DComponent>();
					circleCollider.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					circleCollider.Radius = circleCollider2DComponent["Radius"].as<float>();
					circleCollider.Density = circleCollider2DComponent["Density"].as<float>();
					circleCollider.Friction = circleCollider2DComponent["Friction"].as<float>();
					circleCollider.Restitution = circleCollider2DComponent["Restitution"].as<float>();
					circleCollider.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
				}
			}
		}

		return true;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		// Not implemented
		ENG_CORE_ASSERT(false);

		return false;
	}
}
