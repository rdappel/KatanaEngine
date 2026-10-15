#include "pch.h"
#include "Line.h"

namespace KatanaEngine
{

	Vector2 Line::Lerp(const float value) const
	{
		return Vector2::Lerp(Start, End, value);
	}

	Line& Line::operator=(const Line& other)
	{
		if (this == &other) return *this;

		Start = other.Start;
		End = other.End;
		return *this;
	}

	void Line::Draw(
		const ALLEGRO_COLOR fillColor,
		const ALLEGRO_COLOR strokeColor,
		const float strokeWidth,
		const float offsetX,
		const float offsetY) const
	{
		float sx = Start.X + offsetX;
		float sy = Start.Y + offsetY;
		float ex = End.X + offsetX;
		float ey = End.Y + offsetY;

		al_draw_line(sx, sy, ex, ey, strokeColor, strokeWidth);
	}

	void Line::Translate(const Vector2& offset)
	{
		Start += offset;
		End += offset;
	}
}