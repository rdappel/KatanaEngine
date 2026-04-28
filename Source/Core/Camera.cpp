#include "pch.h"
#include "Camera.h"

namespace KatanaEngine
{
	Camera::Camera()
		: m_viewPosition(Vector2::ZERO)
		, m_viewportSize(1.0f, 1.0f)
	{
	}

	void Camera::SetViewportSize(const Vector2& size)
	{
		m_viewportSize = size;
	}

	void Camera::SetViewportSize(float width, float height)
	{
		m_viewportSize = Vector2(width, height);
	}

	void Camera::SetTarget(const ICameraTarget* target)
	{
		m_pTarget = target;
	}

	void Camera::SetBounds(const IBounded* bounds)
	{
		m_pBounds = bounds;
	}

	Vector2 Camera::GetSnappedViewOffset() const
	{
		return Vector2(
			static_cast<float>(std::round(m_viewPosition.X)),
			static_cast<float>(std::round(m_viewPosition.Y)));
	}

	void Camera::Update()
	{
		if (!m_pTarget) return;

		Vector2 targetPosition = m_pTarget->GetPosition();
		m_viewPosition = targetPosition - (m_viewportSize * 0.5f);
		ClampToBounds();
	}

	Vector2 Camera::WorldToScreen(const Vector2& worldPosition) const
	{
		return worldPosition - m_viewPosition;
	}

	Vector2 Camera::ScreenToWorld(const Vector2& screenPosition) const
	{
		return screenPosition + m_viewPosition;
	}

	void Camera::ClampToBounds()
	{
		if (!m_pBounds) return;

		Rectangle bounds = m_pBounds->GetBounds();
		if (bounds.Width <= 0.0f || bounds.Height <= 0.0f) return;

		if (bounds.Width <= m_viewportSize.X)
		{
			m_viewPosition.X = bounds.GetCenter().X - (m_viewportSize.X * 0.5f);
		}
		else
		{
			float minX = bounds.X;
			float maxX = bounds.X + bounds.Width - m_viewportSize.X;
			m_viewPosition.X = Math::Clamp(m_viewPosition.X, minX, maxX);
		}

		if (bounds.Height <= m_viewportSize.Y)
		{
			m_viewPosition.Y = bounds.GetCenter().Y - (m_viewportSize.Y * 0.5f);
		}
		else
		{
			float minY = bounds.Y;
			float maxY = bounds.Y + bounds.Height - m_viewportSize.Y;
			m_viewPosition.Y = Math::Clamp(m_viewPosition.Y, minY, maxY);
		}
	}
}
