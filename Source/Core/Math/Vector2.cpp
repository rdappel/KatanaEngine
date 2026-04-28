#include "pch.h"

#include "Vector2.h"

namespace KatanaEngine
{
    const Vector2 Vector2::ZERO(0.0f, 0.0f);
    const Vector2 Vector2::ONE(1.0f, 1.0f);
    const Vector2 Vector2::UNIT_X(1.0f, 0.0f);
    const Vector2 Vector2::UNIT_Y(0.0f, 1.0f);


	std::ostream& operator<<(std::ostream& os, const Vector2& vector)
	{
		os << "{" << vector.X << ", " << vector.Y << "}";
		return os;
	}

	std::istream& operator>>(std::istream& is, Vector2& vector)
	{
		char ch1, ch2, ch3;
		is >> ch1 >> vector.X >> ch2 >> vector.Y >> ch3;
		if (ch1 != '{' || ch2 != ',' || ch3 != '}') {
			is.setstate(std::ios::failbit);
		}
		return is;
	}
}
