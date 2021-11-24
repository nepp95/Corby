#include "SceneHierarchyPanel.h"

#include "Engine/Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

namespace Engine {
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		setContext(context);
	}

	void SceneHierarchyPanel::setContext(const Ref<Scene>& context)
	{
		m_context = context;
	}

	void SceneHierarchyPanel::onImGuiRender()
	{
		// -----------------------------------------
		//
		//    Scene Hierarchy Panel
		//
		// -----------------------------------------
		ImGui::Begin("Scene Hierarchy");

		m_context->m_registry.each([&](auto entityID) {
			Entity entity{ entityID, m_context.get() };
			drawEntityNode(entity);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_selectionContext = {};

		ImGui::End();

		// -----------------------------------------
		//
		//    Properties Panel
		//
		// -----------------------------------------
		ImGui::Begin("Properties");

		if (m_selectionContext)
			drawComponents(m_selectionContext);

		ImGui::End();
	}

	void SceneHierarchyPanel::drawEntityNode(Entity entity)
	{
		std::string& tag = entity.getComponent<TagComponent>();

		ImGuiTreeNodeFlags flags = ((m_selectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked()) {
			m_selectionContext = entity;
		}

		if (opened) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());

			if (opened)
				ImGui::TreePop();
			
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::drawComponents(Entity entity)
	{
		if (entity.hasComponent<TagComponent>()) {
			std::string& tag = entity.getComponent<TagComponent>();

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
				tag = std::string(buffer);
		}

		if (entity.hasComponent<TransformComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
				auto& transform = entity.getComponent<TransformComponent>().transform;

				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);
				ImGui::TreePop();
			}
		}

		if (entity.hasComponent<SpriteRendererComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer")) {
				auto& src = entity.getComponent<SpriteRendererComponent>();

				ImGui::ColorEdit4("Color", glm::value_ptr(src.color));
				ImGui::TreePop();
			}
		}

		if (entity.hasComponent<CameraComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera")) {
				auto& cameraComponent = entity.getComponent<CameraComponent>();
				auto& camera = cameraComponent.camera;

				ImGui::Checkbox("Primary", &cameraComponent.primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.getProjectionType()];

				if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
					for (int i = 0; i < 2; i++) {
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];

						if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.setProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.getProjectionType() == SceneCamera::ProjectionType::Perspective) {
					float perspectiveVerticalFOV = glm::degrees(camera.getPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFOV))
						camera.setPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFOV));

					float perspectiveNear = camera.getPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspectiveNear))
						camera.setPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = camera.getPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspectiveFar))
						camera.setPerspectiveFarClip(perspectiveFar);
				}

				if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic) {
					float orthographicSize = camera.getOrthographicSize();
					if (ImGui::DragFloat("Size", &orthographicSize))
						camera.setOrthographicSize(orthographicSize);

					float orthographicNear = camera.getOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthographicNear))
						camera.setOrthographicNearClip(orthographicNear);

					float orthographicFar = camera.getOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthographicFar))
						camera.setOrthographicFarClip(orthographicFar);

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.fixedAspectRatio);
				}

				ImGui::TreePop();
			}
		}
	}
}
