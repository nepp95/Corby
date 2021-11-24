#pragma once

#include "Engine/Renderer/Camera.h"

namespace Engine {
	class SceneCamera : public Camera {
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void setPerspective(float verticalFOV, float nearClip, float farClip);
		void setOrthographic(float size, float nearClip, float farClip);
		void setViewportSize(uint32_t width, uint32_t height);

		float getPerspectiveVerticalFOV() const;
		void setPerspectiveVerticalFOV(float verticalFOV);

		float getPerspectiveNearClip() const;
		void setPerspectiveNearClip(float nearClip);

		float getPerspectiveFarClip() const;
		void setPerspectiveFarClip(float farClip);

		float getOrthographicSize() const;
		void setOrthographicSize(float size);

		float getOrthographicNearClip() const;
		void setOrthographicNearClip(float nearClip);

		float getOrthographicFarClip() const;
		void setOrthographicFarClip(float farClip);

		ProjectionType getProjectionType() const;
		void setProjectionType(ProjectionType type);

	private:
		void recalculateProjection();

	private:
		ProjectionType m_projectionType = ProjectionType::Orthographic;

		float m_perspectiveFOV = glm::radians(45.0f);
		float m_perspectiveNear = 0.01f, m_perspectiveFar = 1000.0f;
		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;
		float m_aspectRatio = 0.0f;
	};
}
