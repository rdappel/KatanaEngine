#include "pch.h"

#include "Image.h"
#include "Texture.h"

namespace KatanaEngine
{
	namespace
	{
		ALLEGRO_BITMAP* CreateMemoryBitmap(int width, int height)
		{
			const int previousFlags = al_get_new_bitmap_flags();
			const int previousFormat = al_get_new_bitmap_format();
			al_set_new_bitmap_flags((previousFlags & ~ALLEGRO_VIDEO_BITMAP) | ALLEGRO_MEMORY_BITMAP);
			al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE);

			ALLEGRO_BITMAP* bitmap = al_create_bitmap(width, height);

			al_set_new_bitmap_format(previousFormat);
			al_set_new_bitmap_flags(previousFlags);
			return bitmap;
		}
	}

	Image::~Image()
	{
		Destroy();
	}

	bool Image::Create(int width, int height)
	{
		Destroy();
		if (width <= 0 || height <= 0) return false;

		m_pBitmap = CreateMemoryBitmap(width, height);
		if (!m_pBitmap) return false;

		ALLEGRO_STATE state;
		al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
		al_set_target_bitmap(m_pBitmap);
		al_clear_to_color(al_map_rgba(0, 0, 0, 0));
		al_restore_state(&state);
		return true;
	}

	void Image::CopyFrom(const Texture& texture, int x, int y)
	{
		if (!m_pBitmap || !texture.IsValid()) return;

		ALLEGRO_STATE state;
		al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
		al_set_target_bitmap(m_pBitmap);
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		al_draw_bitmap(texture.GetBitmap(), static_cast<float>(x), static_cast<float>(y), 0);
		al_restore_state(&state);
	}

	void Image::CopyTo(Texture& texture, int sourceX, int sourceY) const
	{
		if (!m_pBitmap || !texture.IsValid()) return;

		ALLEGRO_STATE state;
		al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);
		al_set_target_bitmap(texture.GetBitmap());
		al_clear_to_color(al_map_rgba(0, 0, 0, 0));
		al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
		al_draw_bitmap_region(
			m_pBitmap,
			static_cast<float>(sourceX),
			static_cast<float>(sourceY),
			static_cast<float>(texture.GetWidth()),
			static_cast<float>(texture.GetHeight()),
			0.0f,
			0.0f,
			0);
		al_restore_state(&state);
	}

	bool Image::Save(const std::string& path) const
	{
		if (!m_pBitmap) return false;

		const int width = GetWidth();
		const int height = GetHeight();
		ALLEGRO_BITMAP* straight = CreateMemoryBitmap(width, height);
		if (!straight) return false;

		ALLEGRO_LOCKED_REGION* source =
			al_lock_bitmap(m_pBitmap, ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE, ALLEGRO_LOCK_READONLY);
		ALLEGRO_LOCKED_REGION* target =
			al_lock_bitmap(straight, ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE, ALLEGRO_LOCK_WRITEONLY);
		if (source && target)
		{
			for (int row = 0; row < height; ++row)
			{
				const auto* in = static_cast<const unsigned char*>(source->data) + row * source->pitch;
				auto* out = static_cast<unsigned char*>(target->data) + row * target->pitch;

				for (int i = 0; i < width * 4; i += 4)
				{
					// Bytes are R, G, B, A. Un-premultiply: c = c * 255 / a (rounded).
					const int alpha = in[i + 3];
					for (int channel = 0; channel < 3; ++channel)
					{
						out[i + channel] = alpha == 0 ? 0
						                              : static_cast<unsigned char>(
															std::min(255, (in[i + channel] * 255 + alpha / 2) / alpha));
					}
					out[i + 3] = static_cast<unsigned char>(alpha);
				}
			}
		}

		if (target) al_unlock_bitmap(straight);
		if (source) al_unlock_bitmap(m_pBitmap);

		const bool saved = source && target && al_save_bitmap(path.c_str(), straight);
		al_destroy_bitmap(straight);
		return saved;
	}

	int Image::GetWidth() const
	{
		return m_pBitmap ? al_get_bitmap_width(m_pBitmap) : 0;
	}

	int Image::GetHeight() const
	{
		return m_pBitmap ? al_get_bitmap_height(m_pBitmap) : 0;
	}

	void Image::Destroy()
	{
		if (!m_pBitmap) return;

		al_destroy_bitmap(m_pBitmap);
		m_pBitmap = nullptr;
	}
}
