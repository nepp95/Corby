#include "engpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_projectionType = ProjectionType::Perspective;
		m_perspectiveFOV = verticalFOV;
		m_perspectiveNear = nearClip;
		m_perspectiveFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_projectionType = ProjectionType::Orthographic;
		m_orthographicSize = size;
		m_orthographicNear = nearClip;
		m_orthographicFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		ENG_CORE_ASSERT(width > 0 && height > 0);
		m_aspectRatio = (float) width / (float) height;
		RecalculateProjection();
	}

	float SceneCamera::GetPerspectiveVerticalFOV() const
	{
		return m_perspectiveFOV;
	}

	void SceneCamera::SetPerspectiveVerticalFOV(float verticalFOV)
	{
		m_perspectiveFOV = verticalFOV;
		RecalculateProjection();
	}

	float SceneCamera::GetPerspectiveNearClip() const
	{
		return m_perspectiveNear;
	}

	void SceneCamera::SetPerspectiveNearClip(float nearClip)
	{
		m_perspectiveNear = nearClip;
		RecalculateProjection();
	}

	float SceneCamera::GetPerspectiveFarClip() const
	{
		return m_perspectiveFar;
	}

	void SceneCamera::SetPerspectiveFarClip(float farClip)
	{
		m_perspectiveFar = farClip;
		RecalculateProjection();
	}

	float SceneCamera::GetOrthographicSize() const
	{
		return m_orthographicSize;
	}

	void SceneCamera::SetOrthographicSize(float size)
	{
		m_orthographicSize = size;
		RecalculateProjection();
	}

	float SceneCamera::GetOrthographicNearClip() const
	{
		return m_orthographicNear;
	}

	void SceneCamera::SetOrthographicNearClip(float nearClip)
	{
		m_orthographicNear = nearClip;
		RecalculateProjection();
	}

	float SceneCamera::GetOrthographicFarClip() const
	{
		return m_orthographicFar;
	}

	void SceneCamera::SetOrthographicFarClip(float farClip)
	{
		m_orthographicFar = farClip;
		RecalculateProjection();
	}

	SceneCamera::ProjectionType SceneCamera::GetProjectionType() const
	{
		return m_projectionType;
	}

	void SceneCamera::SetProjectionType(ProjectionType type)
	{
		m_projectionType = type;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		if (m_projectionType == ProjectionType::Perspective)
		{
			m_projection = glm::perspective(m_perspectiveFOV, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
		} else
		{
			float orthoLeft = -m_orthographicSize * m_aspectRatio * 0.5f;
			float orthoRight = m_orthographicSize * m_aspectRatio * 0.5f;
			float orthoBottom = -m_orthographicSize * 0.5f;
			float orthoTop = m_orthographicSize * 0.5f;

			m_projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_orthographicNear, m_orthographicFar);
		}
	}
}
