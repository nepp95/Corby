#include "engpch.h"
#include "CameraController.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"

namespace Engine
{
	CameraController::CameraController(float aspectRatio, bool rotation)
		: m_aspectRatio(aspectRatio), m_camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio* m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_rotation(rotation)
	{}

	void CameraController::OnUpdate(Timestep ts)
	{
		ENG_PROFILE_FUNCTION();

		// Up / Down
		if (Input::IsKeyPressed(Key::W))
		{
			m_cameraPosition.x += -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		} else if (Input::IsKeyPressed(Key::S))
		{
			m_cameraPosition.x -= -sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}

		// Left / Right
		if (Input::IsKeyPressed(Key::A))
		{
			m_cameraPosition.x -= cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y -= sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		} else if (Input::IsKeyPressed(Key::D))
		{
			m_cameraPosition.x += cos(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
			m_cameraPosition.y += sin(glm::radians(m_cameraRotation)) * m_cameraTranslationSpeed * ts;
		}

		// If rotation available
		if (m_rotation)
		{
			// Rotate left/right
			if (Input::IsKeyPressed(Key::Q))
				m_cameraRotation += m_cameraRotationSpeed * ts;

			if (Input::IsKeyPressed(Key::E))
				m_cameraRotation -= m_cameraRotationSpeed * ts;

			if (m_cameraRotation > 180.0f)
				m_cameraRotation -= 360.0f;
			else if (m_cameraRotation <= -180.0f)
				m_cameraRotation += 360.0f;

			m_camera.SetRotation(m_cameraRotation);
		}

		m_camera.SetPosition(m_cameraPosition);
		m_cameraTranslationSpeed = m_zoomLevel;
	}

	void CameraController::OnEvent(Event& e)
	{
		ENG_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(ENG_BIND_EVENT_FN(CameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(ENG_BIND_EVENT_FN(CameraController::OnWindowResized));
	}

	void CameraController::OnResize(float width, float height)
	{
		m_aspectRatio = width / height;
		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
	}

	void CameraController::CalculateView()
	{
		ENG_PROFILE_FUNCTION();

		m_camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
	}

	bool CameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		ENG_PROFILE_FUNCTION();

		m_zoomLevel -= e.GetYOffset() * 0.25f;
		m_zoomLevel = std::max(m_zoomLevel, 0.25f);
		CalculateView();

		return false;
	}

	bool CameraController::OnWindowResized(WindowResizeEvent& e)
	{
		ENG_PROFILE_FUNCTION();

		OnResize((float) e.GetWidth(), (float) e.GetHeight());

		return false;
	}
}
