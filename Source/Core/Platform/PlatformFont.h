#pragma once

#include "Core/Resource/Font.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

namespace KatanaEngine
{
	struct Font::FontData
	{
		ALLEGRO_FONT* pFont = nullptr;

		~FontData()
		{
			if (pFont)
			{
				al_destroy_font(pFont);
				pFont = nullptr;
			}
		}
	};
}
