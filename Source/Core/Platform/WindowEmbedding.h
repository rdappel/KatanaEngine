#pragma once

struct ALLEGRO_DISPLAY;

namespace KatanaEngine
{
	namespace Platform
	{
		// Size of the parent window's client area (an HWND on Windows). Returns false if unavailable.
		bool GetParentClientSize(void* parentWindow, int& width, int& height);

		// Turns the display's top-level window into a borderless child of parentWindow,
		// positioned at the parent's top-left corner. Windows only; returns false elsewhere.
		bool EmbedDisplay(ALLEGRO_DISPLAY* display, void* parentWindow);

		// Resizes an embedded display and keeps it anchored to the parent's top-left corner.
		void ResizeEmbeddedDisplay(ALLEGRO_DISPLAY* display, int width, int height);
	}
}
