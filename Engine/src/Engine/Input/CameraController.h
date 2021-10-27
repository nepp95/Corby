#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Input/Camera.h"

namespace Engine {
	class CameraController {
	public:
		CameraController(float aspectRatio, bool rotation = false);

		void onUpdate(Timestep timestep);
		void onEvent(Event& event);

		OrthographicCamera& getCamera() { return m_camera; }
		const OrthographicCamera& getCamera() const { return m_camera; }

	private:
		bool onMouseScrolled(MouseScrolledEvent& event);
		bool onWindowResized(WindowResizeEvent& event);

	private:
		OrthographicCamera m_camera;
		glm::vec3 m_cameraPosition = { 0.0f, 0.0f, 0.0f };

		float m_aspectRatio;
		float m_zoomLevel = 1.0f;

		bool m_rotation;
		float m_cameraRotation = 0.0f;
		float m_cameraTranslationSpeed = 5.0f;
		float m_cameraRotationSpeed = 180.0f;
	};
}