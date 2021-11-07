#include <Engine.h>
#include <Engine/Core/Entrypoint.h>

#include "EditorLayer.h"

namespace Engine {
	class Editor : public Application {
	public:
		Editor() : Application("Engine Editor") {
			pushLayer(new EditorLayer());
		}

		~Editor() {}
	};

	Application* createApplication() {
		return new Editor();
	}
}