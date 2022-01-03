#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const;
		void SetPosition(const glm::vec3& position);

		float GetRotation() const;
		void SetRotation(float rotation);

		const glm::mat4 GetProjectionMatrix() const;
		const glm::mat4 GetViewMatrix() const;
		const glm::mat4 GetViewProjectionMatrix() const;

	private:
		void RecalculateViewMatrix();

	private:
		glm::mat4 m_projectionMatrix;
		glm::mat4 m_viewMatrix;
		glm::mat4 m_viewProjectionMatrix;

		glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
		float m_rotation = 0.0f;
	};
}
