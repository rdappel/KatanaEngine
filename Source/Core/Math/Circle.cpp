#include "pch.h"
#include "Circle.h"

namespace KatanaEngine
{
	Circle& Circle::operator=(const Circle& other)
	{
		if (this == &other) return *this;

		X = other.X;
		Y = other.Y;
		Radius = other.Radius;
		return *this;
	}

	void Circle::Draw(
		const ALLEGRO_COLOR fillColor,
		const ALLEGRO_COLOR strokeColor,
		const float strokeWidth,
		const float offsetX,
		const float offsetY) const
	{
		float radius = Radius < 0 ? 0 : Radius;

		float x = X + offsetX;
		float y = Y + offsetY;

		if (fillColor.a > 0) al_draw_filled_circle(x, y, radius, fillColor);
		if (strokeColor.a <= 0) return;
		al_draw_circle(x, y, radius, strokeColor, strokeWidth);
	}

	void Circle::Translate(const Vector2& offset)
	{
		X += offset.X;
		Y += offset.Y;
	}

	void Circle::SetPosition(const Vector2& position)
	{
		X = position.X;
		Y = position.Y;
	}
}