#include "pch.h"

#include "Sound.h"

#include <allegro5/allegro_audio.h>

namespace KatanaEngine
{
	Sound::~Sound()
	{
		if (m_sample)
		{
			al_destroy_sample(m_sample);
			m_sample = nullptr;
		}
	}

	Sound::Sound(Sound&& other) noexcept
		: m_sample(other.m_sample)
	{
		other.m_sample = nullptr;
	}

	Sound& Sound::operator=(Sound&& other) noexcept
	{
		if (this == &other) return *this;

		if (m_sample) al_destroy_sample(m_sample);

		m_sample = other.m_sample;
		other.m_sample = nullptr;
		return *this;
	}

	bool Sound::IsValid() const
	{
		return m_sample != nullptr;
	}

	bool Sound::Play(float volume, float pan, float speed) const
	{
		if (!m_sample) return false;

		return al_play_sample(m_sample, volume, pan, speed, ALLEGRO_PLAYMODE_ONCE, nullptr);
	}

	void Sound::SetSample(ALLEGRO_SAMPLE* sample)
	{
		if (m_sample && m_sample != sample) al_destroy_sample(m_sample);

		m_sample = sample;
	}
}
