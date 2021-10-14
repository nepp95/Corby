#include "engpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Engine {
	RendererAPI* RenderCommand::s_rendererAPI = new OpenGLRendererAPI;
}