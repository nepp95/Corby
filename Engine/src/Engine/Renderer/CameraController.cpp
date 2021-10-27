#include "engpch.h"
#include "CameraController.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine {
	CameraController::CameraController(float aspectRatio, bool rotation) : m_aspectRatio(aspectRatio), m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio* m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_rotation(rotation) {
	}

	void CameraController::onUpdate(Timestep timestep) {
		// Up / Down
		if (Input::isKeyPressed(ENG_KEY_W)) {
			m_cameraPosition.x += -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * timestep;
			m_cameraPosition.y += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * timestep;
		}
		else if (Input::isKeyPressed(ENG_KEY_S)) {
			m_cameraPosition.x -= -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * timestep;
			m_cameraPosition.y -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * timestep;
		}

		// Left / Right
		if (Input::isKeyPressed(ENG_KEY_A)) {
			m_cameraPosition.x -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * timestep;
			m_cameraPosition.y -= sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * timestep;
		}
		else if (Input::isKeyPressed(ENG_KEY_D)) {
			m_cameraPosition.x += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * timestep;
			m_cameraPosition.y += sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * timestep;
		}

		// If rotation available
		if (m_rotation)
		{
			// Rotate left/right
			if (Input::isKeyPressed(ENG_KEY_Q))
				m_cameraRotation += m_cameraRotationSpeed * timestep;

			if (Input::isKeyPressed(ENG_KEY_E))
				m_cameraRotation -= m_cameraRotationSpeed * timestep;

			if (m_cameraRotation > 180.0f)
				m_cameraRotation -= 360.0f;
			else if (m_cameraRotation <= -180.0f)
				m_cameraRotation += 360.0f;

			m_camera.setRotation(m_cameraRotation);
		}

		m_camera.setPosition(m_cameraPosition);
		m_cameraTranslationSpeed = m_zoomLevel;
	}

	void CameraController::onEvent(Event& event) {
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<MouseScrolledEvent>(ENG_BIND_EVENT_FN(CameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizeEvent>(ENG_BIND_EVENT_FN(CameraController::onWindowResized));
	}

	bool CameraController::onMouseScrolled(MouseScrolledEvent& event) {
		m_zoomLevel -= event.getYOffset() * 0.25f;
		m_zoomLevel = std::max(m_zoomLevel, 0.25f);
		m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}

	bool CameraController::onWindowResized(WindowResizeEvent& event) {
		m_aspectRatio = (float)event.getWidth() / (float)event.getHeight();
		m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
		return false;
	}
}