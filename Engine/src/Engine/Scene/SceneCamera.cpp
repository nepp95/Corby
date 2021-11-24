#include "engpch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine {
    SceneCamera::SceneCamera()
    {
        recalculateProjection();
    }

	void SceneCamera::setPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_projectionType = ProjectionType::Perspective;
		m_perspectiveFOV = verticalFOV;
		m_perspectiveNear = nearClip;
		m_perspectiveFar = farClip;
		recalculateProjection();
	}

    void SceneCamera::setOrthographic(float size, float nearClip, float farClip)
    {
		m_projectionType = ProjectionType::Orthographic;
        m_orthographicSize = size;
        m_orthographicNear = nearClip;
        m_orthographicFar = farClip;
        recalculateProjection();
    }

    void SceneCamera::setViewportSize(uint32_t width, uint32_t height)
    {
        m_aspectRatio = (float)width / (float)height;
        recalculateProjection();
    }

	float SceneCamera::getPerspectiveVerticalFOV() const
	{
		return m_perspectiveFOV;
	}

	void SceneCamera::setPerspectiveVerticalFOV(float verticalFOV)
	{
		m_perspectiveFOV = verticalFOV;
		recalculateProjection();
	}

	float SceneCamera::getPerspectiveNearClip() const
	{
		return m_perspectiveNear;
	}

	void SceneCamera::setPerspectiveNearClip(float nearClip)
	{
		m_perspectiveNear = nearClip;
		recalculateProjection();
	}

	float SceneCamera::getPerspectiveFarClip() const
	{
		return m_perspectiveFar;
	}

	void SceneCamera::setPerspectiveFarClip(float farClip)
	{
		m_perspectiveFar = farClip;
		recalculateProjection();
	}

    float SceneCamera::getOrthographicSize() const
    {
        return m_orthographicSize;
    }

    void SceneCamera::setOrthographicSize(float size)
    {
        m_orthographicSize = size;
        recalculateProjection();
    }

	float SceneCamera::getOrthographicNearClip() const
	{
		return m_orthographicNear;
	}

	void SceneCamera::setOrthographicNearClip(float nearClip)
	{
		m_orthographicNear = nearClip;
		recalculateProjection();
	}

	float SceneCamera::getOrthographicFarClip() const
	{
		return m_orthographicFar;
	}

	void SceneCamera::setOrthographicFarClip(float farClip)
	{
		m_orthographicFar = farClip;
		recalculateProjection();
	}

	SceneCamera::ProjectionType SceneCamera::getProjectionType() const
	{
		return m_projectionType;
	}

	void SceneCamera::setProjectionType(ProjectionType type)
	{
		m_projectionType = type;
		recalculateProjection();
	}

    void SceneCamera::recalculateProjection()
    {
		if (m_projectionType == ProjectionType::Perspective) {
			m_projection = glm::perspective(m_perspectiveFOV, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
		}
		else {
			float orthoLeft = -m_orthographicSize * m_aspectRatio * 0.5f;
			float orthoRight = m_orthographicSize * m_aspectRatio * 0.5f;
			float orthoBottom = -m_orthographicSize * 0.5f;
			float orthoTop = m_orthographicSize * 0.5f;
        
			m_projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_orthographicNear, m_orthographicFar);
		}
    }
}
