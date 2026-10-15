#include "pch.h"

#include "RenderTarget.h"
#include "Core/Platform/PlatformColor.h"

namespace KatanaEngine
{
	RenderTarget::~RenderTarget()
	{
		End();
	}

	bool RenderTarget::Create(int width, int height)
	{
		if (width <= 0 || height <= 0) return false;

		End();

		const int previousFlags = al_get_new_bitmap_flags();
		al_set_new_bitmap_flags(previousFlags | ALLEGRO_VIDEO_BITMAP);

		ALLEGRO_BITMAP* bitmap = al_create_bitmap(width, height);
		al_set_new_bitmap_flags(previousFlags);

		if (!bitmap) return false;

		SetBitmap(bitmap);
		return true;
	}

	void RenderTarget::Begin()
	{
		if (!IsValid() || m_isActive) return;

		m_previousTarget = al_get_target_bitmap();
		al_set_target_bitmap(GetBitmap());
		m_isActive = true;
	}

	void RenderTarget::End()
	{
		if (!m_isActive) return;

		if (m_previousTarget) al_set_target_bitmap(m_previousTarget);

		m_previousTarget = nullptr;
		m_isActive = false;
	}

	void RenderTarget::Clear(const Color& color)
	{
		if (!m_isActive) return;

		al_clear_to_color(ToAllegroColor(color));
	}
}
