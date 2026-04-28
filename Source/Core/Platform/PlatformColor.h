#pragma once

#include <memory>

#include <allegro5/allegro.h>

#include "Core/Graphics/Color.h"

namespace KatanaEngine
{
    inline ALLEGRO_COLOR ToAllegroColor(const Color& color)
    {
        return al_map_rgba_f(color.Red, color.Green, color.Blue, color.Alpha);
    }
}
