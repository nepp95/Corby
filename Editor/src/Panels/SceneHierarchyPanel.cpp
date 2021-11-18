#include "SceneHierarchyPanel.h"

#include "Engine/Scene/Components.h"

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
		ImGui::Begin("Scene Hierarchy");

		m_context->m_registry.each([&](auto entityID) {
			Entity entity{ entityID, m_context.get() };
			drawEntityNode(entity);
			});

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
}
