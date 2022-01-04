#include "engpch.h"
#include "RenderCommand.h"

namespace Engine
{
	Scope<RendererAPI> RenderCommand::s_rendererAPI = RendererAPI::Create();
}
