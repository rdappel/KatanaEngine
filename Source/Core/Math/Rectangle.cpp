#include "pch.h"
#include "Rectangle.h"

namespace KatanaEngine
{
	Rectangle& Rectangle::operator=(const Rectangle& other)
	{
		if (this == &other) return *this;

		X = other.X;
		Y = other.Y;
		Width = other.Width;
		Height = other.Height;
		CornerRadius = other.CornerRadius;
		return *this;
	}

	void Rectangle::Translate(const Vector2& offset)
	{
		X += offset.X;
		Y += offset.Y;
	}

	void Rectangle::Draw(
		const ALLEGRO_COLOR fillColor,
		const ALLEGRO_COLOR strokeColor,
		const float strokeWidth,
		const float offsetX,
		const float offsetY) const
	{
		if (Width <= 0 || Height <= 0) return;

		float x = X + offsetX;
		float y = Y + offsetY;

		if (CornerRadius <= 0)
		{
			if (fillColor.a > 0) al_draw_filled_rectangle(x, y, x + Width, y + Height, fillColor);
			if (strokeColor.a <= 0) return;
			al_draw_rectangle(x, y, x + Width, y + Height, strokeColor, strokeWidth);
			return;
		}

		float r = CornerRadius;
		if (fillColor.a > 0) al_draw_filled_rounded_rectangle(x, y, x + Width, y + Height, r, r, fillColor);
		if (strokeColor.a <= 0) return;
		al_draw_rounded_rectangle(x, y, x + Width, y + Height, r, r, strokeColor, strokeWidth);
	}
}
