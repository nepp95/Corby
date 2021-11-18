#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Log.h"
#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"

namespace Engine {
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& context);

		void setContext(const Ref<Scene>& context);

		void onImGuiRender();

	private:
		void drawEntityNode(Entity entity);

	private:
		Ref<Scene> m_context;
		Entity m_selectionContext;
	};
}
