#include <Engine.h>

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Engine::Layer {
public:
	// Construct with 16:9 aspect ratio
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
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.5f, 0.5f, 0.0f,
			-0.5f, 0.5f, 0.0f
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
			uniform mat4 u_transform;
 
			out vec3 v_position;
			out vec4 v_color;

			void main()
			{
				v_position = a_position;
				v_color = a_color;
				gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0f);
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
			uniform mat4 u_transform;
 
			out vec3 v_position;

			void main()
			{
				v_position = a_position;
				gl_Position = u_viewProjection * u_transform * vec4(a_position, 1.0f);
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

	void onUpdate(Engine::Timestep timestep) override {
		ENGINE_INFO("Delta time: {0}s ({1}ms)", timestep.getSeconds(), timestep.getMilliseconds());

		// Up / Down
		if (Engine::Input::isKeyPressed(ENG_KEY_W))
			m_cameraPosition.y += m_cameraMoveSpeed * timestep;
		else if (Engine::Input::isKeyPressed(ENG_KEY_S))
			m_cameraPosition.y -= m_cameraMoveSpeed * timestep;

		// Left / Right
		if (Engine::Input::isKeyPressed(ENG_KEY_A))
			m_cameraPosition.x -= m_cameraMoveSpeed * timestep;
		else if (Engine::Input::isKeyPressed(ENG_KEY_D))
			m_cameraPosition.x += m_cameraMoveSpeed * timestep;

		// Rotate left/right
		if (Engine::Input::isKeyPressed(ENG_KEY_Q))
			m_cameraRotation += m_cameraRotationSpeed * timestep;

		if (Engine::Input::isKeyPressed(ENG_KEY_E))
			m_cameraRotation -= m_cameraRotationSpeed * timestep;

		// Clear screen
		Engine::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::clear();

		// Update camera
		m_camera.setPosition(m_cameraPosition);
		m_camera.setRotation(m_cameraRotation);

		// Begin render
		Engine::Renderer::beginScene(m_camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		for (int y = 0; y < 20; y++) {
			for (int x = 0; x < 20; x++) {
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

				Engine::Renderer::submit(m_blueShader, m_squareVA, transform);
			}
		}

		Engine::Renderer::submit(m_shader, m_vertexArray);

		Engine::Renderer::endScene();
		// End render
	}

	virtual void onImGuiRender() override {
	}

	void onEvent(Engine::Event& event) override {
	}

private:
	Engine::OrthographicCamera m_camera;
	glm::vec3 m_cameraPosition;
	float m_cameraMoveSpeed = 5.0f;
	float m_cameraRotation = 0.0f;
	float m_cameraRotationSpeed = 180.0f;

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