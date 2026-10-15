#pragma once

#include <cstdlib>

namespace KatanaEngine
{
	class Math
	{
	public:
		static const float PI;
		static const float PI_OVER2;
		static const float PI_OVER4;
		static const float INVERSE_PI;
		static const float NORMALIZE_PI_OVER4;
		static const float INVERSE_180;

		static const float FLOAT_MIN;
		static const float FLOAT_MAX;

		static float Lerp(const float start, const float end, const float value, const bool clamp = false);

		static float Clamp(const float value, const float min = 0, const float max = 1);

		static int GetRandomInt(const int min = 0, const int max = RAND_MAX);

		/** @brief Get a random number between zero and one.
		    @return Returns a random floating point number between zero and one. */
		static float GetRandomFloat() { return ((float)rand() / RAND_MAX); }

		static float MapRange(
			const float outputStart,
			const float outputEnd,
			const float inputValue,
			const float inputStart = 0,
			const float inputEnd = 1);

		template<typename T>
		static constexpr T Min(const T& a, const T& b)
		{
			return (a < b) ? a : b;
		}

		template<typename T>
		static constexpr T Max(const T& a, const T& b)
		{
			return (a > b) ? a : b;
		}

		static float Abs(const float value) { return (value < 0) ? -value : value; }

		static float ToRadians(const float degrees) { return degrees * PI * INVERSE_180; }

		static float ToDegrees(const float radians) { return radians * 180 * INVERSE_PI; }
	};
}