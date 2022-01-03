#pragma once

#include "Engine/Core/Timestep.h"
#include "Engine/Events/Event.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Renderer/Camera.h"

#include <glm/glm.hpp>

namespace Engine
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		float GetDistance() const;
		void SetDistance(float distance);

		void SetViewportSize(float width, float height);

		const glm::mat4& GetViewMatrix() const;
		glm::mat4 GetViewProjection() const;

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		const glm::vec3& GetPosition() const;
		glm::quat GetOrientation() const;

		float GetPitch() const;
		float GetYaw() const;

	private:
		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;

		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

	private:
		float m_FOV = 45.0f, m_aspectRatio = 1.778f, m_nearClip = 0.1f, m_farClip = 1000.0f;

		glm::mat4 m_viewMatrix;
		glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_focalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_initialMousePosition = { 0.0f, 0.0f };

		float m_distance = 10.0f;
		float m_pitch = 0.0f, m_yaw = 0.0f;

		float m_viewportWidth = 1280, m_viewportHeight = 720;
	};
}
