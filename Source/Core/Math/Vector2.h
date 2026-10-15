#pragma once

#include "Math.h"

#include <iostream>

namespace KatanaEngine
{
	class Vector2
	{
	public:
		float X;
		float Y;

		Vector2()
			: X(0.0f)
			, Y(0.0f)
		{
		}
		Vector2(float x, float y)
			: X(x)
			, Y(y)
		{
		}
		Vector2(int x, int y)
			: X(static_cast<float>(x))
			, Y(static_cast<float>(y))
		{
		}

		Vector2 operator+(const Vector2& other) const { return Vector2(X + other.X, Y + other.Y); }
		Vector2 operator-(const Vector2& other) const { return Vector2(X - other.X, Y - other.Y); }
		Vector2 operator*(float scalar) const { return Vector2(X * scalar, Y * scalar); }
		Vector2 operator/(float scalar) const { return Vector2(X / scalar, Y / scalar); }

		Vector2& operator+=(const Vector2& other)
		{
			X += other.X;
			Y += other.Y;
			return *this;
		}
		Vector2& operator-=(const Vector2& other)
		{
			X -= other.X;
			Y -= other.Y;
			return *this;
		}
		Vector2& operator*=(float scalar)
		{
			X *= scalar;
			Y *= scalar;
			return *this;
		}
		Vector2& operator/=(float scalar)
		{
			X /= scalar;
			Y /= scalar;
			return *this;
		}

		bool operator==(const Vector2& other) const { return X == other.X && Y == other.Y; }
		bool operator!=(const Vector2& other) const { return !(*this == other); }

		bool IsZero() const { return X == 0 && Y == 0; }

		Vector2 Left() const { return Vector2(-Y, X); }
		Vector2 Right() const { return Vector2(Y, -X); }

		float Length() const { return std::sqrt(X * X + Y * Y); }
		float LengthSquared() const { return X * X + Y * Y; }

		void Normalize()
		{
			float len = Length();
			if (len > 0)
			{
				X /= len;
				Y /= len;
			}
		}

		Vector2 Normalized() const
		{
			float len = Length();
			if (len > 0) return Vector2(X / len, Y / len);
			return Vector2();
		}

		static Vector2 Lerp(const Vector2& start, const Vector2& end, const float value, const bool clamp = true)
		{
			return start + (end - start) * (clamp ? Math::Clamp(value) : value);
		}

		static float Dot(const Vector2& a, const Vector2& b) { return a.X * b.X + a.Y * b.Y; }
		static float Distance(const Vector2& a, const Vector2& b) { return (b - a).Length(); }

		static const Vector2 ZERO;
		static const Vector2 ONE;
		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;
	};

	inline Vector2 operator*(float scalar, const Vector2& vec)
	{
		return vec * scalar;
	}

	std::ostream& operator<<(std::ostream& os, const Vector2& vector);

	std::istream& operator>>(std::istream& is, Vector2& vector);
}
