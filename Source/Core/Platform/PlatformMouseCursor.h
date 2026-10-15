#pragma once

#include "Core/Input/MouseCursor.h"

#include <allegro5/allegro.h>

namespace KatanaEngine
{
	inline ALLEGRO_SYSTEM_MOUSE_CURSOR ToAllegroMouseCursor(MouseCursor cursor)
	{
		switch (cursor)
		{
		case MouseCursor::DEFAULT:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
		case MouseCursor::ARROW:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_ARROW;
		case MouseCursor::BUSY:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY;
		case MouseCursor::QUESTION:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION;
		case MouseCursor::EDIT:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_EDIT;
		case MouseCursor::MOVE:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE;
		case MouseCursor::RESIZE_N:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_N;
		case MouseCursor::RESIZE_W:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_W;
		case MouseCursor::RESIZE_S:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_S;
		case MouseCursor::RESIZE_E:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_E;
		case MouseCursor::RESIZE_NW:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NW;
		case MouseCursor::RESIZE_SW:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SW;
		case MouseCursor::RESIZE_SE:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_SE;
		case MouseCursor::RESIZE_NE:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_RESIZE_NE;
		case MouseCursor::PROGRESS:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_PROGRESS;
		case MouseCursor::PRECISION:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION;
		case MouseCursor::LINK:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK;
		case MouseCursor::ALT_SELECT:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT;
		case MouseCursor::UNAVAILABLE:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE;
		default:
			return ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT;
		}
	}
}
