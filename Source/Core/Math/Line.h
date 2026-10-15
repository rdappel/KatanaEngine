#pragma once

#include "Vector2.h"
#include "Core/Interfaces/IPrimitive.h"

#include <allegro5/allegro.h>

namespace KatanaEngine
{
	class Line : public IPrimitive
	{
		friend class SpriteBatch;

	public:
		Vector2 Start;
		Vector2 End;

		Line() {}

		Line(Vector2 start, Vector2 end)
			: Start(start)
			, End(end)
		{
		}

		Line(float x1, float y1, float x2, float y2)
			: Line(Vector2(x1, y1), Vector2(x2, y2))
		{
		}

		Line(const Line& other)
			: Line(other.Start, other.End)
		{
		}

		Vector2 Lerp(const float value) const;

		Line& operator=(const Line& other);

		virtual IPrimitive::Type GetType() const override { return IPrimitive::Type::Line; }

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