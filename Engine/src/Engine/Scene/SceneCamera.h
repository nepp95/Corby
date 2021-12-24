#pragma once

#include "Engine/Renderer/Camera.h"

namespace Engine
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		void SetOrthographic(float size, float nearClip, float farClip);
		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const;
		void SetPerspectiveVerticalFOV(float verticalFOV);

		float GetPerspectiveNearClip() const;
		void SetPerspectiveNearClip(float nearClip);

		float GetPerspectiveFarClip() const;
		void SetPerspectiveFarClip(float farClip);

		float GetOrthographicSize() const;
		void SetOrthographicSize(float size);

		float GetOrthographicNearClip() const;
		void SetOrthographicNearClip(float nearClip);

		float GetOrthographicFarClip() const;
		void SetOrthographicFarClip(float farClip);

		ProjectionType GetProjectionType() const;
		void SetProjectionType(ProjectionType type);

	private:
		void RecalculateProjection();

	private:
		ProjectionType m_projectionType = ProjectionType::Orthographic;

		float m_perspectiveFOV = glm::radians(45.0f);
		float m_perspectiveNear = 0.01f, m_perspectiveFar = 1000.0f;
		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;
		float m_aspectRatio = 0.0f;
	};
}
