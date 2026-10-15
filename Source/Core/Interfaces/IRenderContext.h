#pragma once

#include "Core/Math/Vector2.h"

namespace KatanaEngine
{
	class IRenderContext
	{
	public:
		virtual ~IRenderContext() {}

		virtual Vector2 GetViewOffset() const = 0;
		virtual Vector2 GetSnappedViewOffset() const = 0;
		virtual Vector2 WorldToScreen(const Vector2& worldPosition) const = 0;
		virtual Vector2 ScreenToWorld(const Vector2& screenPosition) const = 0;
		virtual float GetZoom() const { return 1.0f; }
	};
}
