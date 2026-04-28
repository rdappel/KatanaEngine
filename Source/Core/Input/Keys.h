#pragma once

namespace KatanaEngine
{
    enum class Keys
    {
        NONE = 0,

        // Letters
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        // Numbers
        D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,

        // Function keys
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        // Arrow keys
        UP, DOWN, LEFT, RIGHT,

        // Special keys
        SPACE,
        ENTER,
        ESCAPE,
        TAB,
        BACKSPACE,
        DELETE,
        INSERT,
        HOME,
        END,
        PAGE_UP,
        PAGE_DOWN,

        // Modifiers
        LEFT_SHIFT,
        RIGHT_SHIFT,
        LEFT_CONTROL,
        RIGHT_CONTROL,
        LEFT_ALT,
        RIGHT_ALT,

        // Numpad
        NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4,
        NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9,
        NUMPAD_ADD,
        NUMPAD_SUBTRACT,
        NUMPAD_MULTIPLY,
        NUMPAD_DIVIDE,
        NUMPAD_DECIMAL,
        NUMPAD_ENTER,

        // Other
        COMMA,
        PERIOD,
        SLASH,
        SEMICOLON,
        QUOTE,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        BACKSLASH,
        MINUS,
        EQUALS,
        TILDE,

        KEY_COUNT
    };
}
