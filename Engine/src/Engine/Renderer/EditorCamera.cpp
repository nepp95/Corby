#include "engpch.h"
#include "EditorCamera.h"

#include "Engine/Core/Input.h"
#include "Engine/Core/KeyCodes.h"
#include "Engine/Core/MouseCodes.h"

#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_aspectRatio(aspectRatio), m_nearClip(nearClip), m_farClip(farClip), Camera(glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip))
	{
		updateView();
	}

	void EditorCamera::onUpdate(Timestep ts)
	{
		if (Input::isKeyPressed(Key::LeftAlt))
		{
			const glm::vec2& mouse{ Input::getMouseX(), Input::getMouseY() };
			glm::vec2 delta = (mouse - m_initialMousePosition) * 0.003f;
			m_initialMousePosition = mouse;

			if (Input::isMouseButtonPressed(Mouse::ButtonMiddle))
				mousePan(delta);
			else if (Input::isMouseButtonPressed(Mouse::ButtonLeft))
				mouseRotate(delta);
			else if (Input::isMouseButtonPressed(Mouse::ButtonRight))
				mouseZoom(delta.y);
		}

		updateView();
	}

	void EditorCamera::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>(ENG_BIND_EVENT_FN(EditorCamera::onMouseScroll));
	}

	float EditorCamera::getDistance() const
	{
		return m_distance;
	}

	void EditorCamera::setDistance(float distance)
	{
		m_distance = distance;
	}

	void EditorCamera::setViewportSize(float width, float height)
	{
		m_viewportWidth = width;
		m_viewportHeight = height;
		updateProjection();
	}

	const glm::mat4& EditorCamera::getViewMatrix() const
	{
		return m_viewMatrix;
	}

	glm::mat4 EditorCamera::getViewProjection() const
	{
		return m_projection * m_viewMatrix;
	}

	glm::vec3 EditorCamera::getUpDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getRightDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getForwardDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	const glm::vec3& EditorCamera::getPosition() const
	{
		return m_position;
	}

	glm::quat EditorCamera::getOrientation() const
	{
		return glm::quat(glm::vec3(-m_pitch, -m_yaw, 0.0f));
	}

	float EditorCamera::getPitch() const
	{
		return m_pitch;
	}

	float EditorCamera::getYaw() const
	{
		return m_yaw;
	}

	void EditorCamera::updateProjection()
	{
		m_aspectRatio = m_viewportWidth / m_viewportHeight;
		m_projection = glm::perspective(glm::radians(m_FOV), m_aspectRatio, m_nearClip, m_farClip);
	}

	void EditorCamera::updateView()
	{
		m_position = calculatePosition();

		glm::quat orientation = getOrientation();
		m_viewMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(orientation);
		m_viewMatrix = glm::inverse(m_viewMatrix);
	}

	bool EditorCamera::onMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.getYOffset() * 0.1f;
		mouseZoom(delta);
		updateView();

		return false;
	}

	void EditorCamera::mousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = panSpeed();
		m_focalPoint += -getRightDirection() * delta.x * xSpeed * m_distance;
		m_focalPoint += getUpDirection() * delta.y * ySpeed * m_distance;
	}

	void EditorCamera::mouseRotate(const glm::vec2& delta)
	{
		float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
		m_yaw += yawSign * delta.x * rotationSpeed();
		m_pitch += delta.y * rotationSpeed();
	}

	void EditorCamera::mouseZoom(float delta)
	{
		m_distance -= delta * zoomSpeed();

		if (m_distance < 1.0f)
		{
			m_focalPoint += getForwardDirection();
			m_distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::calculatePosition() const
	{
		return m_focalPoint - getForwardDirection() * m_distance;
	}

	std::pair<float, float> EditorCamera::panSpeed() const
	{
		float x = std::min(m_viewportWidth / 1000.0f, 2.4f);
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_viewportHeight / 1000.0f, 2.4f);
		float yFactor = 0.0366f * (y * y) - 0.1778f * y * 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::rotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::zoomSpeed() const
	{
		float distance = m_distance * 0.2f;
		distance = std::max(distance, 0.0f);

		float speed = distance * distance;
		speed = std::min(speed, 100.0f);

		return speed;
	}
}
