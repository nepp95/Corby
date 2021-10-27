#include "Sandbox2D.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Platform/OpenGL/OpenGLShader.h>

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_cameraController(1280.0f / 720.0f) {
}

void Sandbox2D::onAttach() {
	m_squareVA = Engine::VertexArray::create();

	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f
	};

	Engine::Ref<Engine::VertexBuffer> squareVB;
	squareVB = Engine::VertexBuffer::create(squareVertices, sizeof(squareVertices));
	squareVB->setLayout({
		{ Engine::ShaderDataType::Float3, "a_position" }
		});
	m_squareVA->addVertexBuffer(squareVB);

	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	Engine::Ref<Engine::IndexBuffer> squareIB;
	squareIB = Engine::IndexBuffer::create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
	m_squareVA->setIndexBuffer(squareIB);

	m_flatColorShader = Engine::Shader::create("assets/shaders/FlatColor.glsl");
}

void Sandbox2D::onDetach() {
}

void Sandbox2D::onUpdate(Engine::Timestep timestep) {
	// -----------------------------------------
	//
	//    Update
	//
	// -----------------------------------------
	m_cameraController.onUpdate(timestep);

	// -----------------------------------------
	//
	//    Render
	//
	// -----------------------------------------
	Engine::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	Engine::RenderCommand::clear();

	Engine::Renderer::beginScene(m_cameraController.getCamera());

	std::dynamic_pointer_cast<Engine::OpenGLShader>(m_flatColorShader)->bind();
	std::dynamic_pointer_cast<Engine::OpenGLShader>(m_flatColorShader)->uploadUniformFloat4("u_color", m_squareColor);

	Engine::Renderer::submit(m_flatColorShader, m_squareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

	Engine::Renderer::endScene();
}

void Sandbox2D::onImGuiRender() {
	ImGui::Begin("Settings");
	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));
	ImGui::End();
}

void Sandbox2D::onEvent(Engine::Event& e) {
	m_cameraController.onEvent(e);
}
