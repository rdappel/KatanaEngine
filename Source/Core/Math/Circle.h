#pragma once

#include "Vector2.h"
#include "Core/Interfaces/IPrimitive.h"
#include "Core/Interfaces/IPositionable.h"

namespace KatanaEngine
{
	class Circle : public IPrimitive, public IPositionable
	{
		friend class SpriteBatch;

	public:
		float X = 0;
		float Y = 0;
		float Radius = 1;

		Circle(const float x = 0, const float y = 0, const float radius = 1)
			: X(x), Y(y), Radius(radius) {
		}

		Circle(const Vector2& position, const float radius = 1)
			: Circle(position.X, position.Y, radius) {
		}

		Circle(const Circle& other)
			: Circle(other.X, other.Y, other.Radius) {
		}

		Circle(Circle&& other) noexcept
			: Circle(other.X, other.Y, other.Radius) {
		}

		Circle& operator=(const Circle& other);

		virtual IPrimitive::Type GetType() const override { return IPrimitive::Type::Circle; }

		virtual void Translate(const Vector2& offset) override;

		virtual Vector2 GetPosition() const { return Vector2(X, Y); }

		virtual void SetPosition(const Vector2& position);

	private:
		virtual void Draw(const ALLEGRO_COLOR fillColor, const ALLEGRO_COLOR strokeColor,
			const float strokeWidth, const float offsetX, const float offsetY) const override;
	};
}