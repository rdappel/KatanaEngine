#pragma once

#include "Resource.h"
#include "Core/GameTime.h"

#include <string>

struct ALLEGRO_AUDIO_STREAM;

namespace KatanaEngine
{
    class ResourceManager;

    class Music : public Resource
    {
    public:
        ~Music();

        Music(Music&& other) noexcept;
        Music& operator=(Music&& other) noexcept;

        Music(const Music&) = delete;
        Music& operator=(const Music&) = delete;

        bool IsValid() const;
        bool IsPlaying() const;

        bool Play(bool loop = false, float loopPauseSeconds = 0.0f);
        void Stop();
        void Pause();
        void Resume();
        void Update(const GameTime& gameTime);
        void SetVolume(float volume);

    private:
        friend class ResourceManager;

        Music() = default;
        bool LoadFromFile(const std::string& path);
        bool Restart();

        ALLEGRO_AUDIO_STREAM* m_stream = nullptr;
        bool m_loop = false;
        bool m_shouldPlay = false;
        bool m_waitingForLoopRestart = false;
        float m_loopPauseSeconds = 0.0f;
        float m_loopPauseRemaining = 0.0f;
        float m_volume = 1.0f;
    };
}
