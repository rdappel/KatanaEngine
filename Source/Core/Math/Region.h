#pragma once

#include "Vector2.h"

namespace KatanaEngine
{
    class Region
    {
    public:
        int X;
        int Y;
        int Width;
        int Height;

        Region() : X(0), Y(0), Width(0), Height(0) {}
        Region(int x, int y, int width, int height) : X(x), Y(y), Width(width), Height(height) {}

        int Left() const { return X; }
        int Right() const { return X + Width; }
        int Top() const { return Y; }
        int Bottom() const { return Y + Height; }

        Vector2 GetPosition() const { return Vector2(static_cast<float>(X), static_cast<float>(Y)); }
        Vector2 GetSize() const { return Vector2(static_cast<float>(Width), static_cast<float>(Height)); }
        Vector2 GetCenter() const { return Vector2(X + Width / 2.0f, Y + Height / 2.0f); }

        bool Contains(int px, int py) const
        {
            return px >= X && px < X + Width && py >= Y && py < Y + Height;
        }

        bool Contains(const Vector2& point) const
        {
            return Contains(static_cast<int>(point.X), static_cast<int>(point.Y));
        }

        bool Intersects(const Region& other) const
        {
            return X < other.X + other.Width && X + Width > other.X &&
                Y < other.Y + other.Height && Y + Height > other.Y;
        }

        bool operator==(const Region& other) const
        {
            return X == other.X && Y == other.Y && Width == other.Width && Height == other.Height;
        }

        bool operator!=(const Region& other) const { return !(*this == other); }

        static const Region EMPTY;
    };
}
