#pragma once

#include "Keys.h"
#include "MouseButton.h"
#include "Core/Math/Vector2.h"

#include <array>

namespace KatanaEngine
{

    class InputState
    {
    public:
        InputState();

        void Update();

        // Keyboard - Current frame state
        bool IsKeyDown(Keys key) const;
        bool IsKeyUp(Keys key) const;

        // Keyboard - State transitions (this frame vs last frame)
        bool IsKeyPressed(Keys key) const;   // Just pressed this frame
        bool IsKeyReleased(Keys key) const;  // Just released this frame

        // Mouse - Position
        Vector2 GetMousePosition() const { return m_mousePosition; }
        int GetMouseX() const { return static_cast<int>(m_mousePosition.X); }
        int GetMouseY() const { return static_cast<int>(m_mousePosition.Y); }

        // Mouse - Current frame state
        bool IsMouseButtonDown(MouseButton button) const;
        bool IsMouseButtonUp(MouseButton button) const;

        // Mouse - State transitions
        bool IsMouseButtonPressed(MouseButton button) const;   // Just pressed this frame
        bool IsMouseButtonReleased(MouseButton button) const;  // Just released this frame

    private:
        static const int KEY_COUNT = static_cast<int>(Keys::KEY_COUNT);
        static const int MOUSE_BUTTON_COUNT = static_cast<int>(MouseButton::BUTTON_COUNT);

        std::array<bool, KEY_COUNT> m_currentKeyState;
        std::array<bool, KEY_COUNT> m_previousKeyState;

        std::array<bool, MOUSE_BUTTON_COUNT> m_currentMouseState;
        std::array<bool, MOUSE_BUTTON_COUNT> m_previousMouseState;
        Vector2 m_mousePosition;
    };
}
