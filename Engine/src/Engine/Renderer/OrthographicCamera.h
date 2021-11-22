#pragma once

#include <glm/glm.hpp>

namespace Engine {
	class OrthographicCamera {
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void setProjection(float left, float right, float bottom, float top);

		const glm::vec3& getPosition() const;
		void setPosition(const glm::vec3& position);

		float getRotation() const;
		void setRotation(float rotation);

		const glm::mat4 getProjectionMatrix() const;
		const glm::mat4 getViewMatrix() const;
		const glm::mat4 getViewProjectionMatrix() const;

	private:
		void recalculateViewMatrix();

	private:
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_viewProjectionMatrix;

		glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
		float m_rotation = 0.0f;
	};
}