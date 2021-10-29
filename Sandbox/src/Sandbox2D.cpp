#include "Sandbox2D.h"

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>

template<typename Fn>
class Timer {
public:
	Timer(const char* name, Fn&& func) : m_name(name), m_func(func), m_stopped(false) {
		m_startTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer() {
		if (!m_stopped)
			stop();
	}

	void stop() {
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_stopped = true;

		float duration = (end - start) * 0.001f;
		m_func({ m_name, duration });
	}

private:
	const char* m_name;
	Fn m_func;
	std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
	bool m_stopped;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_profileResults.push_back(profileResult); })

Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_cameraController(1280.0f / 720.0f) {
}

void Sandbox2D::onAttach() {
	m_checkerboardTexture = Engine::Texture2D::create("assets/textures/Checkerboard.png");
}

void Sandbox2D::onDetach() {
}

void Sandbox2D::onUpdate(Engine::Timestep timestep) {
	PROFILE_SCOPE("Sandbox2D::onUpdate");

	// -----------------------------------------
	//
	//    Update
	//
	// -----------------------------------------
	{
		PROFILE_SCOPE("CameraContoller::onUpdate");
		m_cameraController.onUpdate(timestep);
	}

	// -----------------------------------------
	//
	//    Render
	//
	// -----------------------------------------
	{
		PROFILE_SCOPE("Renderer Prep");
		Engine::RenderCommand::setClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Engine::RenderCommand::clear();
	}

	{
		PROFILE_SCOPE("Renderer Draw");
		Engine::Renderer2D::beginScene(m_cameraController.getCamera());
		Engine::Renderer2D::drawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Engine::Renderer2D::drawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		Engine::Renderer2D::drawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_checkerboardTexture);
		Engine::Renderer2D::endScene();
	}

}

void Sandbox2D::onImGuiRender() {
	ImGui::Begin("Settings");

	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));

	for (auto& result : m_profileResults) {
		char label[50];
		strcpy(label, "%.3fms ");
		strcat(label, result.name);
		ImGui::Text(label, result.time);
	}

	m_profileResults.clear();

	ImGui::End();
}

void Sandbox2D::onEvent(Engine::Event& e) {
	m_cameraController.onEvent(e);
}
