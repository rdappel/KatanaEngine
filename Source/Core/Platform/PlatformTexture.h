#pragma once

#include <allegro5/allegro.h>
#include "Core/Resource/Texture.h"

namespace KatanaEngine
{
	struct Texture::TextureData
	{
		ALLEGRO_BITMAP* pBitmap = nullptr;

		~TextureData()
		{
			if (pBitmap)
			{
				al_destroy_bitmap(pBitmap);
				pBitmap = nullptr;
			}
		}
	};
}
