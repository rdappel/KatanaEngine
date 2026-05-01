#pragma once

#include "Resource.h"

#include <string>

struct ALLEGRO_SAMPLE;

namespace KatanaEngine
{
    class ResourceManager;

    class Sound : public Resource
    {
    public:
        ~Sound();

        Sound(Sound&& other) noexcept;
        Sound& operator=(Sound&& other) noexcept;

        Sound(const Sound&) = delete;
        Sound& operator=(const Sound&) = delete;

        bool IsValid() const;
        bool Play(float volume = 1.0f, float pan = 0.0f, float speed = 1.0f) const;

    private:
        friend class ResourceManager;

        Sound() = default;
        void SetSample(ALLEGRO_SAMPLE* sample);

        ALLEGRO_SAMPLE* m_sample = nullptr;
    };
}
