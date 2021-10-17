#include <Engine.h>

#include "imgui/imgui.h"

class ExampleLayer : public Engine::Layer {
public:
	ExampleLayer() : Layer("Example"), m_camera(-1.6f, 1.6f, -0.9f, 0.9f), m_cameraPosition(1.0f) {
		// -----------------------------------------
		//
		//    Triangle
		//
		// -----------------------------------------
		m_vertexArray.reset(Engine::VertexArray::create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			0.0f, 0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		std::shared_ptr<Engine::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Engine::VertexBuffer::create(vertices, sizeof(vertices)));

		Engine::BufferLayout layout = {
			{ Engine::ShaderDataType::Float3, "a_position" },
			{ Engine::ShaderDataType::Float4, "a_color" }
		};

		vertexBuffer->setLayout(layout);
		m_vertexArray->addVertexBuffer(vertexBuffer);

		unsigned int indices[3] = {
			0, 1, 2
		};

		std::shared_ptr<Engine::IndexBuffer> indexBuffer;
		indexBuffer.reset(Engine::IndexBuffer::create(indices, sizeof(indices) / sizeof(unsigned int)));
		m_vertexArray->setIndexBuffer(indexBuffer);

		// -----------------------------------------
		//
		//    Square
		//
		// -----------------------------------------
		m_squareVA.reset(Engine::VertexArray::create());

		float squareVertices[3 * 4] = {
			-0.75f, -0.75f, 0.0f,
			0.75f, -0.75f, 0.0f,
			0.75f, 0.75f, 0.0f,
			-0.75f, 0.75f, 0.0f
		};

		std::shared_ptr<Engine::VertexBuffer> squareVB;
		squareVB.reset(Engine::VertexBuffer::create(squareVertices, sizeof(squareVertices)));

		squareVB->setLayout({
			{ Engine::ShaderDataType::Float3, "a_position" }
			});
		m_squareVA->addVertexBuffer(squareVB);

		unsigned int squareIndices[6] = {
			0, 1, 2, 2, 3, 0
		};

		std::shared_ptr<Engine::IndexBuffer> squareIB;
		squareIB.reset(Engine::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(unsigned int)));
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

		m_shader.reset(new Engine::Shader(vertexSrc, fragmentSrc));

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

		m_blueShader.reset(new Engine::Shader(blueShaderVertexSrc, blueShaderFragmentSrc));
	}

	void onUpdate() override {
		// Clear screen
		Engine::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::clear();

		m_camera.setPosition(m_cameraPosition);
		//m_camera.setRotation(45.0f);

		Engine::Renderer::beginScene(m_camera);

		Engine::Renderer::submit(m_blueShader, m_squareVA);
		Engine::Renderer::submit(m_shader, m_vertexArray);

		Engine::Renderer::endScene();
	}

	virtual void onImGuiRender() override {
	}

	void onEvent(Engine::Event& event) override {
		Engine::EventDispatcher dispatcher(event);
		dispatcher.dispatch<Engine::KeyPressedEvent>(ENGINE_BIND_EVENT_FN(ExampleLayer::onKeyPressedEvent));
	}

	bool onKeyPressedEvent(Engine::KeyPressedEvent& event) {
		if (event.getKeyCode() == ENG_KEY_W)
			m_cameraPosition.y += m_cameraSpeed;

		if (event.getKeyCode() == ENG_KEY_A)
			m_cameraPosition.x -= m_cameraSpeed;

		if (event.getKeyCode() == ENG_KEY_S)
			m_cameraPosition.y -= m_cameraSpeed;

		if (event.getKeyCode() == ENG_KEY_D)
			m_cameraPosition.x += m_cameraSpeed;

		return false;
	}

private:
	Engine::OrthographicCamera m_camera;
	glm::vec3 m_cameraPosition;
	float m_cameraSpeed = 0.1f;

	std::shared_ptr<Engine::Shader> m_shader;
	std::shared_ptr<Engine::VertexArray> m_vertexArray;
	std::shared_ptr<Engine::Shader> m_blueShader;
	std::shared_ptr<Engine::VertexArray> m_squareVA;
};

class Sandbox : public Engine::Application {
public:
	Sandbox() {
		pushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};

Engine::Application* Engine::createApplication() {
	return new Sandbox();
}