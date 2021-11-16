#pragma once

#include "Engine/Renderer/Camera.h"

namespace Engine {
	class SceneCamera : public Camera {
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		void setOrthographic(float size, float nearClip, float farClip);
		void setViewportSize(uint32_t width, uint32_t height);

		float getOrthographicSize() const;
		void setOrthographicSize(float size);

	private:
		void recalculateProjection();

	private:
		float m_orthographicSize = 10.0f;
		float m_orthographicNear = -1.0f, m_orthographicFar = 1.0f;
		float m_aspectRatio = 0.0f;
	};
}