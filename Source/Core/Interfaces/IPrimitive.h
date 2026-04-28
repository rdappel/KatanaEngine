#pragma once

#include <allegro5/allegro_primitives.h>

namespace KatanaEngine
{
	class IPrimitive
	{
	public:
		enum class Type
		{
			Point,
			Line,
			Rectangle,
			Circle,
			Spline
		};

		virtual ~IPrimitive() { }

		virtual Type GetType() const = 0;

		virtual void Translate(const Vector2& offset) = 0;

		virtual void Draw(const ALLEGRO_COLOR fillColor, const ALLEGRO_COLOR strokeColor,
			const float strokeWidth, const float offsetX, const float offsetY) const = 0;

	};
}