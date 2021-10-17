#include "engpch.h"
#include "Application.h"

#include "Log.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"

namespace Engine {
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_instance = nullptr;

	Application::Application() : m_camera(-1.6f, 1.6f, -0.9f, 0.9f) {
		ENGINE_CORE_ASSERT(!s_instance, "Application already exists!");
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::create());
		m_window->setEventCallback(BIND_EVENT_FN(onEvent));

		m_imGuiLayer = new ImGuiLayer();
		//pushOverlay(m_imGuiLayer);

		// -----------------------------------------
		//
		//    Triangle
		//
		// -----------------------------------------
		m_vertexArray.reset(VertexArray::create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

		BufferLayout layout = {
			{ ShaderDataType::Float3, "a_position" },
			{ ShaderDataType::Float4, "a_color" }
		};

		vertexBuffer->setLayout(layout);
		m_vertexArray->addVertexBuffer(vertexBuffer);

		unsigned int indices[3] = {
			0, 1, 2
		};

		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(unsigned int)));
		m_vertexArray->setIndexBuffer(indexBuffer);

		// -----------------------------------------
		//
		//    Square
		//
		// -----------------------------------------
		m_squareVA.reset(VertexArray::create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			0.75f, -0.75f, 0.0f,
			0.75f, 0.75f, 0.0f,
			-0.75f, 0.75f, 0.0f
		};

		std::shared_ptr<VertexBuffer> squareVB;
		squareVB.reset(VertexBuffer::create(squareVertices, sizeof(squareVertices)));

		squareVB->setLayout({
			{ ShaderDataType::Float3, "a_position" }
			});
		m_squareVA->addVertexBuffer(squareVB);

		unsigned int squareIndices[6] = {
			0, 1, 2, 2, 3, 0
		};

		std::shared_ptr<IndexBuffer> squareIB;
		squareIB.reset(IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(unsigned int)));
		m_squareVA->setIndexBuffer(squareIB);

		// -----------------------------------------
		//
		//    Shaders
		//
		// -----------------------------------------
		std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;
			layout(location = 1) in vec4 a_color;

			uniform mat4 u_viewProjection;
 
			out vec3 v_position;
			out vec4 v_color;

			void main()
			{
				v_position = a_position;
				v_color = a_color;
				gl_Position = u_viewProjection * vec4(a_position, 1.0f);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;
 
			in vec3 v_position;
			in vec4 v_color;

			void main()
			{
				color = vec4(v_position * 0.5 + 0.5, 1.0);
				color = v_color;
			}
		)";

		m_shader.reset(new Shader(vertexSrc, fragmentSrc));

		std::string blueShaderVertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_position;

			uniform mat4 u_viewProjection;
 
			out vec3 v_position;

			void main()
			{
				v_position = a_position;
				gl_Position = u_viewProjection * vec4(a_position, 1.0f);
			}
		)";

		std::string blueShaderFragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;
 
			in vec3 v_position;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		)";

		m_blueShader.reset(new Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	Application::~Application() {
	}

	void Application::run() {
		while (m_running) {
			// Clear screen
			RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::clear();

			// Play with camera :)
			//m_camera.setPosition({ 0.5f, 0.5f, 0.5f });
			//m_camera.setRotation(45.0f);

			Renderer::beginScene(m_camera);

			Renderer::submit(m_blueShader, m_squareVA);
			Renderer::submit(m_shader, m_vertexArray);

			Renderer::endScene();

			// Layers onUpdate
			for (Layer* layer : m_layerStack)
				layer->onUpdate();

			// ImGui
			//m_imGuiLayer->begin();
			//for (Layer* layer : m_layerStack) {
			//	layer->onImGuiRender();
			//}
			//m_imGuiLayer->end();
			//

			m_window->onUpdate();
		}
	}

	void Application::onEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(onWindowClose));

		for (auto it = m_layerStack.end(); it != m_layerStack.begin();) {
			(*--it)->onEvent(e);

			if (e.handled) {
				break;
			}
		}
	}

	void Application::pushLayer(Layer* layer) {
		m_layerStack.pushLayer(layer);
	}

	void Application::pushOverlay(Layer* layer) {
		m_layerStack.pushOverlay(layer);
	}

	bool Application::onWindowClose(WindowCloseEvent& e) {
		m_running = false;
		return true;
	}
}