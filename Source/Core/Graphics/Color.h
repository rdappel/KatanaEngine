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

        Color operator*(float alphaScale) const
        {
            Color result = *this;
            result.Alpha *= alphaScale;
            return result;
        }

        Color& operator*=(float alphaScale)
        {
            Alpha *= alphaScale;
            return *this;
        }

        Color& operator=(const Color& color)
        {
            if (this != &color)
            {
                Red = color.Red;
                Green = color.Green;
                Blue = color.Blue;
                Alpha = color.Alpha;
            }
            return *this;
        }

        static const Color TRANSPARENT;

        static const Color BLACK;
        static const Color WHITE;

        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color YELLOW;
        static const Color CYAN;
        static const Color MAGENTA;

        static const Color ALICE_BLUE;
        static const Color ANTIQUE_WHITE;
        static const Color AQUA;
        static const Color AQUAMARINE;
        static const Color AZURE;
        static const Color BEIGE;
        static const Color BISQUE;
        static const Color BLANCHED_ALMOND;
        static const Color BLUE_VIOLET;
        static const Color BROWN;
        static const Color BURLY_WOOD;
        static const Color CADET_BLUE;
        static const Color CHARTREUSE;
        static const Color CHOCOLATE;
        static const Color CORAL;
        static const Color CORNFLOWER_BLUE;
        static const Color CRIMSON;
        static const Color DARK_BLUE;
        static const Color DARK_CYAN;
        static const Color DARK_GOLDENROD;
        static const Color DARK_GRAY;
        static const Color DARK_GREEN;
        static const Color DARK_KHAKI;
        static const Color DARK_MAGENTA;
        static const Color DARK_OLIVE_GREEN;
        static const Color DARK_ORANGE;
        static const Color DARK_RED;
        static const Color DARK_SALMON;
        static const Color DARK_SEA_GREEN;
        static const Color DARK_SLATE_BLUE;
        static const Color DARK_SLATE_GRAY;
        static const Color DARK_TURQUOISE;
        static const Color DARK_VIOLET;
        static const Color DEEP_PINK;
        static const Color DIM_GRAY;
        static const Color DODGER_BLUE;
        static const Color FIREBRICK;
        static const Color FLORAL_WHITE;
        static const Color FOREST_GREEN;
        static const Color GAINSBORO;
        static const Color GHOST_WHITE;
        static const Color GOLD;
        static const Color GOLDENROD;
        static const Color GRAY;
        static const Color GREEN_YELLOW;
        static const Color HONEYDEW;
        static const Color HOT_PINK;
        static const Color INDIAN_RED;
        static const Color INDIGO;
        static const Color IVORY;
        static const Color KHAKI;
        static const Color LAVENDER;
        static const Color LAVENDER_BLUSH;
        static const Color LAWN_GREEN;
        static const Color LEMON_CHIFFON;
        static const Color LIGHT_CORAL;
        static const Color LIGHT_CYAN;
        static const Color LIGHT_GOLDENROD_YELLOW;
        static const Color LIGHT_BLUE;
        static const Color LIGHT_GRAY;
        static const Color LIGHT_GREEN;
        static const Color LIGHT_PINK;
        static const Color LIGHT_SALMON;
        static const Color LIGHT_SEA_GREEN;
        static const Color LIGHT_SKY_BLUE;
        static const Color LIGHT_SLATE_GRAY;
        static const Color LIGHT_STEEL_BLUE;
        static const Color LIGHT_YELLOW;
        static const Color LIME;
        static const Color LIME_GREEN;
        static const Color LINEN;
        static const Color MAROON;
        static const Color MEDIUM_AQUAMARINE;
        static const Color MEDIUM_BLUE;
        static const Color MEDIUM_ORCHID;
        static const Color MEDIUM_PURPLE;
        static const Color MEDIUM_SEA_GREEN;
        static const Color MEDIUM_SLATE_BLUE;
        static const Color MEDIUM_SPRING_GREEN;
        static const Color MEDIUM_TURQUOISE;
        static const Color MEDIUM_VIOLET_RED;
        static const Color MIDNIGHT_BLUE;
        static const Color MINT_CREAM;
        static const Color MISTY_ROSE;
        static const Color MOCCASIN;
        static const Color NAVAJO_WHITE;
        static const Color NAVY;
        static const Color OLD_LACE;
        static const Color OLIVE;
        static const Color OLIVE_DRAB;
        static const Color ORANGE;
        static const Color ORANGE_RED;
        static const Color ORCHID;
        static const Color PAPAYA_WHIP;
        static const Color PALE_GOLDENROD;
        static const Color PALE_GREEN;
        static const Color PALE_TURQUOISE;
        static const Color PEACH_PUFF;
        static const Color PERU;
        static const Color PLUM;
        static const Color POWDER_BLUE;
        static const Color REBECCA_PURPLE;
        static const Color PINK;
        static const Color PURPLE;
        static const Color ROSY_BROWN;
        static const Color ROYAL_BLUE;
        static const Color SADDLE_BROWN;
        static const Color SALMON;
        static const Color SANDY_BROWN;
        static const Color SEA_GREEN;
        static const Color SEASHELL;
        static const Color SIENNA;
        static const Color SILVER;
        static const Color SNOW;
        static const Color TAN;
        static const Color TEAL;
        static const Color THISTLE;
        static const Color TURQUOISE;
        static const Color SKY_BLUE;
        static const Color SLATE_GRAY;
        static const Color SPRING_GREEN;
        static const Color STEEL_BLUE;
        static const Color TOMATO;
        static const Color VIOLET;
        static const Color WHEAT;

    };

    inline Color operator*(float alphaScale, const Color& color)
    {
        return color * alphaScale;
    }
}
