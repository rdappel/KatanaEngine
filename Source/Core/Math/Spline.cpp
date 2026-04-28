#include "pch.h"
#include "Spline.h"

namespace KatanaEngine
{
	Vector2 Spline::Lerp(const float value) const
	{
		float t1 = value;
		float t2 = t1 * t1;
		float u1 = 1 - t1;
		float u2 = u1 * u1;

		Vector2 a = u2 * u1 * Start;
		Vector2 b = 3 * u2 * t1 * StartControl;
		Vector2 c = 3 * u1 * t2 * EndControl;
		Vector2 d = t2 * t1 * End;

		return a + b + c + d;
	}

	Spline& Spline::operator=(const Spline& other)
	{
		if (this == &other) return *this;

		Start = other.Start;
		StartControl = other.StartControl;
		EndControl = other.EndControl;
		End = other.End;
		return *this;
	}

	void Spline::Draw(const ALLEGRO_COLOR fillColor, const ALLEGRO_COLOR strokeColor,
		const float strokeWidth, const float offsetX, const float offsetY) const
	{
		float points[8] = {
			Start.X + offsetX, Start.Y + offsetY,
			StartControl.X + offsetX, StartControl.Y + offsetY,
			EndControl.X + offsetX, EndControl.Y + offsetY,
			End.X + offsetX, End.Y + offsetY
		};

		al_draw_spline(points, strokeColor, strokeWidth);
	}

	void Spline::Translate(const Vector2& offset)
	{
		Start += offset;
		StartControl += offset;
		EndControl += offset;
		End += offset;
	}
}