#include <Engine.h>
#include <Engine/Core/Entrypoint.h>

#include "EditorLayer.h"

namespace Engine
{
	class Editor : public Application
	{
	public:
		Editor(const ApplicationSpecification& specification)
			: Application(specification)
		{
			PushLayer(new EditorLayer());
		}

		~Editor() {}
	};

	Application* CreateApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.Name = "CorbyEd";
		spec.CommandLineArgs = args;

		return new Editor(spec);
	}
}
