#include <Engine.h>
#include <Engine/Core/Entrypoint.h>

#include "EditorLayer.h"

namespace Engine
{
	class Editor : public Application
	{
	public:
		Editor(ApplicationCommandLineArgs args)
			: Application("Engine Editor", args)
		{
			PushLayer(new EditorLayer());
		}

		~Editor() {}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		return new Editor(args);
	}
}
