#pragma once

#include <cstdint>

#ifdef TRANSPARENT
#undef TRANSPARENT
#endif

namespace KatanaEngine
{
    struct Color
    {
        float Red;
        float Green;
        float Blue;
        float Alpha;

        Color() : Red(0.0f), Green(0.0f), Blue(0.0f), Alpha(1.0f) { }
        Color(float r, float g, float b, float a = 1.0f) : Red(r), Green(g), Blue(b), Alpha(a) { }
        Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
            : Red(r / 255.0f), Green(g / 255.0f), Blue(b / 255.0f), Alpha(a / 255.0f) {
        }

        bool operator==(const Color& other) const
        {
            return Red == other.Red && Green == other.Green && Blue == other.Blue && Alpha == other.Alpha;
        }

        bool operator!=(const Color& other) const { return !(*this == other); }

        static const Color BLACK;
        static const Color WHITE;
        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color YELLOW;
        static const Color CYAN;
        static const Color MAGENTA;
        static const Color TRANSPARENT;
        static const Color CORNFLOWER_BLUE;
    };
}
