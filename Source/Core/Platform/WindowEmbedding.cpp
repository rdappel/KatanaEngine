#include "pch.h"

#include "WindowEmbedding.h"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <allegro5/allegro_windows.h>

namespace KatanaEngine
{
	namespace Platform
	{
		bool GetParentClientSize(void* parentWindow, int& width, int& height)
		{
			RECT rect{};
			if (!parentWindow || !GetClientRect(static_cast<HWND>(parentWindow), &rect)) return false;

			width = rect.right - rect.left;
			height = rect.bottom - rect.top;
			return width > 0 && height > 0;
		}

		bool EmbedDisplay(ALLEGRO_DISPLAY* display, void* parentWindow)
		{
			if (!display || !parentWindow) return false;

			const HWND window = al_get_win_window_handle(display);
			const HWND parent = static_cast<HWND>(parentWindow);
			if (!window || !IsWindow(parent)) return false;

			const LONG_PTR topLevelStyles = WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			const LONG_PTR style = (GetWindowLongPtr(window, GWL_STYLE) & ~topLevelStyles) | WS_CHILD | WS_CLIPSIBLINGS;
			const LONG_PTR exStyle = (GetWindowLongPtr(window, GWL_EXSTYLE) & ~(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE)) | WS_EX_NOPARENTNOTIFY;

			SetWindowLongPtr(window, GWL_STYLE, style);
			SetWindowLongPtr(window, GWL_EXSTYLE, exStyle);
			if (!SetParent(window, parent)) return false;

			const int width = al_get_display_width(display);
			const int height = al_get_display_height(display);
			SetWindowPos(window, nullptr, 0, 0, width, height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
			return true;
		}

		void ResizeEmbeddedDisplay(ALLEGRO_DISPLAY* display, int width, int height)
		{
			if (!display || width <= 0 || height <= 0) return;

			al_resize_display(display, width, height);
			SetWindowPos(al_get_win_window_handle(display), nullptr, 0, 0, width, height, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
}

#else

namespace KatanaEngine
{
	namespace Platform
	{
		bool GetParentClientSize(void*, int&, int&) { return false; }
		bool EmbedDisplay(ALLEGRO_DISPLAY*, void*) { return false; }
		void ResizeEmbeddedDisplay(ALLEGRO_DISPLAY*, int, int) { }
	}
}

#endif
