#include "engpch.h"
#include "CameraController.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine
{
	CameraController::CameraController(float aspectRatio, bool rotation) : m_aspectRatio(aspectRatio), m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio* m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_rotation(rotation)
	{}

	void CameraController::onUpdate(Timestep ts)
	{
		ENG_PROFILE_FUNCTION();

		// Up / Down
		if (Input::isKeyPressed(Key::W))
		{
			m_cameraPosition.x += -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		} else if (Input::isKeyPressed(Key::S))
		{
			m_cameraPosition.x -= -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}

		// Left / Right
		if (Input::isKeyPressed(Key::A))
		{
			m_cameraPosition.x -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y -= sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		} else if (Input::isKeyPressed(Key::D))
		{
			m_cameraPosition.x += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y += sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}

		// If rotation available
		if (m_rotation)
		{
			// Rotate left/right
			if (Input::isKeyPressed(Key::Q))
				m_cameraRotation += m_cameraRotationSpeed * ts;

			if (Input::isKeyPressed(Key::E))
				m_cameraRotation -= m_cameraRotationSpeed * ts;

			if (m_cameraRotation > 180.0f)
				m_cameraRotation -= 360.0f;
			else if (m_cameraRotation <= -180.0f)
				m_cameraRotation += 360.0f;

			m_camera.setRotation(m_cameraRotation);
		}

		m_camera.setPosition(m_cameraPosition);
		m_cameraTranslationSpeed = m_zoomLevel;
	}

	void CameraController::onEvent(Event& e)
	{
		ENG_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(ENG_BIND_EVENT_FN(CameraController::onMouseScrolled));
		dispatcher.dispatch<WindowResizeEvent>(ENG_BIND_EVENT_FN(CameraController::onWindowResized));
	}

	void CameraController::onResize(float width, float height)
	{
		m_aspectRatio = width / height;
		m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
	}

	void CameraController::calculateView()
	{
		ENG_PROFILE_FUNCTION();

		m_camera.setProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
	}

	bool CameraController::onMouseScrolled(MouseScrolledEvent& e)
	{
		ENG_PROFILE_FUNCTION();

		m_zoomLevel -= e.getYOffset() * 0.25f;
		m_zoomLevel = std::max(m_zoomLevel, 0.25f);
		calculateView();

		return false;
	}

	bool CameraController::onWindowResized(WindowResizeEvent& e)
	{
		ENG_PROFILE_FUNCTION();

		onResize((float) e.getWidth(), (float) e.getHeight());

		return false;
	}
}
