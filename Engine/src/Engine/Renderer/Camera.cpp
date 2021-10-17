#include "engpch.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top) : m_projectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_viewMatrix(1.0f) {
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	const glm::vec3& OrthographicCamera::getPosition() const {
		return m_position;
	}

	void OrthographicCamera::setPosition(const glm::vec3& position) {
		m_position = position;
		recalculateViewMatrix();
	}

	float OrthographicCamera::getRotation() const {
		return m_rotation;
	}

	void OrthographicCamera::setRotation(float rotation) {
		m_rotation = rotation;
		recalculateViewMatrix();
	}

	const glm::mat4 OrthographicCamera::getProjectionMatrix() const {
		return m_projectionMatrix;
	}

	const glm::mat4 OrthographicCamera::getViewMatrix() const {
		return m_viewMatrix;
	}

	const glm::mat4 OrthographicCamera::getViewProjectionMatrix() const {
		return m_viewProjectionMatrix;
	}

	void OrthographicCamera::recalculateViewMatrix() {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
			glm::rotate(glm::mat4(1.0), glm::radians(m_rotation), glm::vec3(0, 0, 1));

		m_viewMatrix = glm::inverse(transform);
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}
}