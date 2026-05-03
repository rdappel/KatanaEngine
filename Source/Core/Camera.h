#pragma once

#include "Core/Interfaces/IBounded.h"
#include "Core/Interfaces/IRenderContext.h"
#include "Core/Interfaces/ICameraTarget.h"
#include "Core/Math/Vector2.h"

namespace KatanaEngine
{
	class Camera : public IRenderContext
	{
	public:
		Camera();

		void SetViewportSize(const Vector2& size);
		void SetViewportSize(float width, float height);

		void SetTarget(const ICameraTarget* target);
		void SetBounds(const IBounded* bounds);

		void ZoomIn();
		void ZoomOut();
		float GetZoom() const override { return m_zoom; }

		void Update();

		Vector2 GetPosition() const { return m_viewPosition; }
		Vector2 GetViewportSize() const { return m_viewportSize; }
		Vector2 GetCenterPosition() const { return m_viewPosition + m_viewportSize * 0.5f; }

		Vector2 GetViewOffset() const override { return m_viewPosition; }
		Vector2 GetSnappedViewOffset() const override;
		Vector2 WorldToScreen(const Vector2& worldPosition) const override;
		Vector2 ScreenToWorld(const Vector2& screenPosition) const override;

	private:
		void ClampToBounds();

		static constexpr float k_zoomLevels[] = { 0.24f, 0.50f, 1.00f, 1.50f, 2.00f };
		static constexpr int k_defaultZoomIndex = 4; // 200%

		Vector2 m_viewPosition;
		Vector2 m_viewportSize;
		const ICameraTarget* m_pTarget = nullptr;
		const IBounded* m_pBounds = nullptr;
		int m_zoomIndex = k_defaultZoomIndex;
		float m_zoom = k_zoomLevels[k_defaultZoomIndex];
	};
}
