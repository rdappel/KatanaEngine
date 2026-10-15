#pragma once

#include "Vector2.h"
#include "Core/Interfaces/IPrimitive.h"

namespace KatanaEngine
{
	class Spline : public IPrimitive
	{
		friend class SpriteBatch;

	public:
		Vector2 Start;
		Vector2 StartControl;
		Vector2 EndControl;
		Vector2 End;

		Spline() {}

		Spline(Vector2 start, Vector2 startControl, Vector2 endControl, Vector2 end)
			: Start(start)
			, StartControl(startControl)
			, EndControl(endControl)
			, End(end)
		{
		}

		Spline(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
			: Spline(Vector2(x1, y1), Vector2(x2, y2), Vector2(x3, y3), Vector2(x4, y4))
		{
		}

		Spline(const Spline& other)
			: Spline(other.Start, other.StartControl, other.EndControl, other.End)
		{
		}

		Spline(Spline&& other) noexcept
			: Spline(other.Start, other.StartControl, other.EndControl, other.End)
		{
		}

		Vector2 Lerp(const float value) const;

		Spline& operator=(const Spline& other);

		virtual IPrimitive::Type GetType() const override { return IPrimitive::Type::Spline; }

		virtual void Translate(const Vector2& offset) override;

	private:
		virtual void Draw(
			const ALLEGRO_COLOR fillColor,
			const ALLEGRO_COLOR strokeColor,
			const float strokeWidth,
			const float offsetX,
			const float offsetY) const override;
	};
}