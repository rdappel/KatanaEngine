#pragma once

#include "Core/Input/SystemMouseCursor.h"

#include <allegro5/allegro.h>

namespace KatanaEngine
{
    inline ALLEGRO_SYSTEM_MOUSE_CURSOR ToAllegroSystemMouseCursor(SystemMouseCursor cursor)
    {
        switch (cursor)
        {
        case SystemMouseCursor::DEFAULT:      return ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
        case SystemMouseCursor::ARROW:        return ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW;
        case SystemMouseCursor::BUSY:         return ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY;
        case SystemMouseCursor::QUESTION:     return ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION;
        case SystemMouseCursor::EDIT:         return ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT;
        case SystemMouseCursor::MOVE:         return ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE;
        case SystemMouseCursor::RESIZE_N:     return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N;
        case SystemMouseCursor::RESIZE_W:     return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W;
        case SystemMouseCursor::RESIZE_S:     return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S;
        case SystemMouseCursor::RESIZE_E:     return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E;
        case SystemMouseCursor::RESIZE_NW:    return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW;
        case SystemMouseCursor::RESIZE_SW:    return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW;
        case SystemMouseCursor::RESIZE_SE:    return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE;
        case SystemMouseCursor::RESIZE_NE:    return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE;
        case SystemMouseCursor::PROGRESS:     return ALLEGRO_SYSTEM_MOUSE_CURSOR_PROGRESS;
        case SystemMouseCursor::PRECISION:    return ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
        case SystemMouseCursor::LINK:         return ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK;
        case SystemMouseCursor::ALT_SELECT:   return ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT;
        case SystemMouseCursor::UNAVAILABLE:  return ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE;
        default:                              return ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
        }
    }
}
