#pragma once

#include "Math.h"
#include "Vector2.h"
#include "Region.h"
#include "Core/Interfaces/IPrimitive.h"

namespace KatanaEngine
{
	class Rectangle : public IPrimitive
	{
		friend class SpriteBatch;

	public:
		float X = 0;
		float Y = 0;
		float Width = 0;
		float Height = 0;
		float CornerRadius = 0;

		Rectangle(const float x, const float y, const float width, const float height, const float cornerRadius = 0)
			: X(x)
			, Y(y)
			, Width(width)
			, Height(height)
			, CornerRadius(cornerRadius)
		{
			float smallerHalf = Math::Min(width, height) / 2;
			if (CornerRadius > smallerHalf) CornerRadius = smallerHalf;
		}

		Rectangle(const Region& region, const float cornerRadius = 0)
			: Rectangle(region.X, region.Y, region.Width, region.Height, cornerRadius)
		{
		}

		Rectangle(const Rectangle& other)
			: Rectangle(other.X, other.Y, other.Width, other.Height, other.CornerRadius)
		{
		}

		Rectangle& operator=(const Rectangle& other);

		virtual IPrimitive::Type GetType() const override { return IPrimitive::Type::Rectangle; }

		virtual void Translate(const Vector2& offset) override;

		virtual Vector2 GetMinimum() const { return Vector2(X, Y); }

		virtual Vector2 GetSize() const { return Vector2(Width, Height); }

		virtual Vector2 GetMaximum() const { return GetMinimum() + GetSize(); }

		virtual Vector2 GetCenter() const { return GetMinimum() + GetSize() / 2; }

	private:
		virtual void Draw(
			const ALLEGRO_COLOR fillColor,
			const ALLEGRO_COLOR strokeColor,
			const float strokeWidth,
			const float offsetX,
			const float offsetY) const override;
	};
}