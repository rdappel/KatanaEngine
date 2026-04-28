#include "pch.h"
#include "Math.h"

#include <limits>

namespace KatanaEngine
{
	const float Math::PI = 3.14159265359f;
	const float Math::PI_OVER2 = Math::PI / 2;
	const float Math::PI_OVER4 = Math::PI / 4;
	const float Math::INVERSE_PI = 1.0f / Math::PI;
	const float Math::NORMALIZE_PI_OVER4 = 0.70710678119f;
	const float Math::INVERSE_180 = 1.0f / 180;

	const float Math::FLOAT_MIN = std::numeric_limits<float>::min();
	const float Math::FLOAT_MAX = std::numeric_limits<float>::max();

	float Math::Clamp(const float value, const float min, const float max)
	{
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	float Math::Lerp(const float start, const float end,
		const float value, const bool clamp)
	{
		return start + (end - start) * (clamp ? Clamp(value) : value);
	}

	int Math::GetRandomInt(const int min, const int max)
	{
		return min + (rand() % (max - min));
	}

	float Math::MapRange(const float outputStart, const float outputEnd, const float inputValue,
		const float inputStart, const float inputEnd)
	{
		float inputRange = inputEnd - inputStart;
		float outputRange = outputEnd - outputStart;
		float scale = (inputValue - inputStart) / inputRange;
		return scale * outputRange + outputStart;
	}
}