#include "pch.h"

#include "Music.h"

#include <allegro5/allegro_audio.h>

namespace KatanaEngine
{
	namespace
	{
		constexpr size_t STREAM_BUFFER_COUNT = 4;
		constexpr unsigned int STREAM_BUFFER_SAMPLES = 2048;
	}

	Music::~Music()
	{
		if (m_stream)
		{
			al_detach_audio_stream(m_stream);
			al_destroy_audio_stream(m_stream);
			m_stream = nullptr;
		}
	}

	Music::Music(Music&& other) noexcept
		: m_stream(other.m_stream)
		, m_loop(other.m_loop)
		, m_shouldPlay(other.m_shouldPlay)
		, m_waitingForLoopRestart(other.m_waitingForLoopRestart)
		, m_loopPauseSeconds(other.m_loopPauseSeconds)
		, m_loopPauseRemaining(other.m_loopPauseRemaining)
		, m_volume(other.m_volume)
	{
		other.m_stream = nullptr;
	}

	Music& Music::operator=(Music&& other) noexcept
	{
		if (this == &other) return *this;

		if (m_stream)
		{
			al_detach_audio_stream(m_stream);
			al_destroy_audio_stream(m_stream);
		}

		m_stream = other.m_stream;
		m_loop = other.m_loop;
		m_shouldPlay = other.m_shouldPlay;
		m_waitingForLoopRestart = other.m_waitingForLoopRestart;
		m_loopPauseSeconds = other.m_loopPauseSeconds;
		m_loopPauseRemaining = other.m_loopPauseRemaining;
		m_volume = other.m_volume;

		other.m_stream = nullptr;
		return *this;
	}

	bool Music::IsValid() const
	{
		return m_stream != nullptr;
	}

	bool Music::IsPlaying() const
	{
		return m_stream && al_get_audio_stream_playing(m_stream);
	}

	bool Music::Play(bool loop, float loopPauseSeconds)
	{
		if (!m_stream) return false;

		m_loop = loop;
		m_loopPauseSeconds = std::max(0.0f, loopPauseSeconds);

		if (IsPlaying()) return true;

		m_shouldPlay = true;
		m_loopPauseRemaining = 0.0f;
		m_waitingForLoopRestart = false;

		bool modeLoop = m_loop && m_loopPauseSeconds <= 0.0f;

		ALLEGRO_PLAYMODE playmode = modeLoop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE;

		al_set_audio_stream_playmode(m_stream, playmode);

		return Restart();
	}

	void Music::Stop()
	{
		if (!m_stream) return;

		al_set_audio_stream_playing(m_stream, false);
		al_rewind_audio_stream(m_stream);
		m_shouldPlay = false;
		m_waitingForLoopRestart = false;
		m_loopPauseRemaining = 0.0f;
	}

	void Music::Pause()
	{
		if (!m_stream) return;

		al_set_audio_stream_playing(m_stream, false);
		m_shouldPlay = false;
	}

	void Music::Resume()
	{
		if (!m_stream) return;

		m_shouldPlay = true;
		if (!m_waitingForLoopRestart) al_set_audio_stream_playing(m_stream, true);
	}

	void Music::Update(const GameTime& gameTime)
	{
		if (!m_stream || !m_shouldPlay || !m_loop || m_loopPauseSeconds <= 0.0f) return;

		if (m_waitingForLoopRestart)
		{
			m_loopPauseRemaining -= static_cast<float>(gameTime.ElapsedTime);
			if (m_loopPauseRemaining <= 0.0f) Restart();
			return;
		}

		if (!al_get_audio_stream_playing(m_stream))
		{
			m_waitingForLoopRestart = true;
			m_loopPauseRemaining = m_loopPauseSeconds;
		}
	}

	void Music::SetVolume(float volume)
	{
		m_volume = std::max(0.0f, volume);

		if (!m_stream) return;
		al_set_audio_stream_gain(m_stream, m_volume);
	}

	bool Music::LoadFromFile(const std::string& path)
	{
		if (m_stream)
		{
			al_detach_audio_stream(m_stream);
			al_destroy_audio_stream(m_stream);
			m_stream = nullptr;
		}

		m_stream = al_load_audio_stream(path.c_str(), STREAM_BUFFER_COUNT, STREAM_BUFFER_SAMPLES);
		if (!m_stream) return false;

		al_set_audio_stream_gain(m_stream, m_volume);
		al_set_audio_stream_playmode(m_stream, ALLEGRO_PLAYMODE_ONCE);
		al_set_audio_stream_playing(m_stream, false);

		if (!al_attach_audio_stream_to_mixer(m_stream, al_get_default_mixer()))
		{
			al_destroy_audio_stream(m_stream);
			m_stream = nullptr;
			return false;
		}

		return true;
	}

	bool Music::Restart()
	{
		if (!m_stream) return false;

		al_set_audio_stream_playing(m_stream, false);
		if (!al_rewind_audio_stream(m_stream)) al_seek_audio_stream_secs(m_stream, 0.0);

		m_waitingForLoopRestart = false;
		m_loopPauseRemaining = 0.0f;
		return al_set_audio_stream_playing(m_stream, true);
	}
}
