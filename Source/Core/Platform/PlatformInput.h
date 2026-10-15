#pragma once

#include "Core/Input/Keys.h"

#include <allegro5/allegro.h>

namespace KatanaEngine
{
	inline int ToAllegroKey(Keys key)
	{
		switch (key)
		{
			// Letters
		case Keys::A:
			return ALLEGRO_KEY_A;
		case Keys::B:
			return ALLEGRO_KEY_B;
		case Keys::C:
			return ALLEGRO_KEY_C;
		case Keys::D:
			return ALLEGRO_KEY_D;
		case Keys::E:
			return ALLEGRO_KEY_E;
		case Keys::F:
			return ALLEGRO_KEY_F;
		case Keys::G:
			return ALLEGRO_KEY_G;
		case Keys::H:
			return ALLEGRO_KEY_H;
		case Keys::I:
			return ALLEGRO_KEY_I;
		case Keys::J:
			return ALLEGRO_KEY_J;
		case Keys::K:
			return ALLEGRO_KEY_K;
		case Keys::L:
			return ALLEGRO_KEY_L;
		case Keys::M:
			return ALLEGRO_KEY_M;
		case Keys::N:
			return ALLEGRO_KEY_N;
		case Keys::O:
			return ALLEGRO_KEY_O;
		case Keys::P:
			return ALLEGRO_KEY_P;
		case Keys::Q:
			return ALLEGRO_KEY_Q;
		case Keys::R:
			return ALLEGRO_KEY_R;
		case Keys::S:
			return ALLEGRO_KEY_S;
		case Keys::T:
			return ALLEGRO_KEY_T;
		case Keys::U:
			return ALLEGRO_KEY_U;
		case Keys::V:
			return ALLEGRO_KEY_V;
		case Keys::W:
			return ALLEGRO_KEY_W;
		case Keys::X:
			return ALLEGRO_KEY_X;
		case Keys::Y:
			return ALLEGRO_KEY_Y;
		case Keys::Z:
			return ALLEGRO_KEY_Z;

			// Numbers
		case Keys::D0:
			return ALLEGRO_KEY_0;
		case Keys::D1:
			return ALLEGRO_KEY_1;
		case Keys::D2:
			return ALLEGRO_KEY_2;
		case Keys::D3:
			return ALLEGRO_KEY_3;
		case Keys::D4:
			return ALLEGRO_KEY_4;
		case Keys::D5:
			return ALLEGRO_KEY_5;
		case Keys::D6:
			return ALLEGRO_KEY_6;
		case Keys::D7:
			return ALLEGRO_KEY_7;
		case Keys::D8:
			return ALLEGRO_KEY_8;
		case Keys::D9:
			return ALLEGRO_KEY_9;

			// Function keys
		case Keys::F1:
			return ALLEGRO_KEY_F1;
		case Keys::F2:
			return ALLEGRO_KEY_F2;
		case Keys::F3:
			return ALLEGRO_KEY_F3;
		case Keys::F4:
			return ALLEGRO_KEY_F4;
		case Keys::F5:
			return ALLEGRO_KEY_F5;
		case Keys::F6:
			return ALLEGRO_KEY_F6;
		case Keys::F7:
			return ALLEGRO_KEY_F7;
		case Keys::F8:
			return ALLEGRO_KEY_F8;
		case Keys::F9:
			return ALLEGRO_KEY_F9;
		case Keys::F10:
			return ALLEGRO_KEY_F10;
		case Keys::F11:
			return ALLEGRO_KEY_F11;
		case Keys::F12:
			return ALLEGRO_KEY_F12;

			// Arrow keys
		case Keys::UP:
			return ALLEGRO_KEY_UP;
		case Keys::DOWN:
			return ALLEGRO_KEY_DOWN;
		case Keys::LEFT:
			return ALLEGRO_KEY_LEFT;
		case Keys::RIGHT:
			return ALLEGRO_KEY_RIGHT;

			// Special keys
		case Keys::SPACE:
			return ALLEGRO_KEY_SPACE;
		case Keys::ENTER:
			return ALLEGRO_KEY_ENTER;
		case Keys::ESCAPE:
			return ALLEGRO_KEY_ESCAPE;
		case Keys::TAB:
			return ALLEGRO_KEY_TAB;
		case Keys::BACKSPACE:
			return ALLEGRO_KEY_BACKSPACE;
		case Keys::DELETE:
			return ALLEGRO_KEY_DELETE;
		case Keys::INSERT:
			return ALLEGRO_KEY_INSERT;
		case Keys::HOME:
			return ALLEGRO_KEY_HOME;
		case Keys::END:
			return ALLEGRO_KEY_END;
		case Keys::PAGE_UP:
			return ALLEGRO_KEY_PGUP;
		case Keys::PAGE_DOWN:
			return ALLEGRO_KEY_PGDN;

			// Modifiers
		case Keys::LEFT_SHIFT:
			return ALLEGRO_KEY_LSHIFT;
		case Keys::RIGHT_SHIFT:
			return ALLEGRO_KEY_RSHIFT;
		case Keys::LEFT_CONTROL:
			return ALLEGRO_KEY_LCTRL;
		case Keys::RIGHT_CONTROL:
			return ALLEGRO_KEY_RCTRL;
		case Keys::LEFT_ALT:
			return ALLEGRO_KEY_ALT;
		case Keys::RIGHT_ALT:
			return ALLEGRO_KEY_ALTGR;

			// Numpad
		case Keys::NUMPAD_0:
			return ALLEGRO_KEY_PAD_0;
		case Keys::NUMPAD_1:
			return ALLEGRO_KEY_PAD_1;
		case Keys::NUMPAD_2:
			return ALLEGRO_KEY_PAD_2;
		case Keys::NUMPAD_3:
			return ALLEGRO_KEY_PAD_3;
		case Keys::NUMPAD_4:
			return ALLEGRO_KEY_PAD_4;
		case Keys::NUMPAD_5:
			return ALLEGRO_KEY_PAD_5;
		case Keys::NUMPAD_6:
			return ALLEGRO_KEY_PAD_6;
		case Keys::NUMPAD_7:
			return ALLEGRO_KEY_PAD_7;
		case Keys::NUMPAD_8:
			return ALLEGRO_KEY_PAD_8;
		case Keys::NUMPAD_9:
			return ALLEGRO_KEY_PAD_9;
		case Keys::NUMPAD_ADD:
			return ALLEGRO_KEY_PAD_PLUS;
		case Keys::NUMPAD_SUBTRACT:
			return ALLEGRO_KEY_PAD_MINUS;
		case Keys::NUMPAD_MULTIPLY:
			return ALLEGRO_KEY_PAD_ASTERISK;
		case Keys::NUMPAD_DIVIDE:
			return ALLEGRO_KEY_PAD_SLASH;
		case Keys::NUMPAD_DECIMAL:
			return ALLEGRO_KEY_PAD_DELETE;
		case Keys::NUMPAD_ENTER:
			return ALLEGRO_KEY_PAD_ENTER;

			// Other
		case Keys::COMMA:
			return ALLEGRO_KEY_COMMA;
		case Keys::PERIOD:
			return ALLEGRO_KEY_FULLSTOP;
		case Keys::SLASH:
			return ALLEGRO_KEY_SLASH;
		case Keys::SEMICOLON:
			return ALLEGRO_KEY_SEMICOLON;
		case Keys::QUOTE:
			return ALLEGRO_KEY_QUOTE;
		case Keys::LEFT_BRACKET:
			return ALLEGRO_KEY_OPENBRACE;
		case Keys::RIGHT_BRACKET:
			return ALLEGRO_KEY_CLOSEBRACE;
		case Keys::BACKSLASH:
			return ALLEGRO_KEY_BACKSLASH;
		case Keys::MINUS:
			return ALLEGRO_KEY_MINUS;
		case Keys::EQUALS:
			return ALLEGRO_KEY_EQUALS;
		case Keys::TILDE:
			return ALLEGRO_KEY_TILDE;

		default:
			return -1;
		}
	}
}
