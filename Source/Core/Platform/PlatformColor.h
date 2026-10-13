#pragma once

#include <memory>

#include <allegro5/allegro.h>

#include "Core/Graphics/Color.h"

namespace KatanaEngine
{
    // Engine colors are straight (non-premultiplied) alpha. Allegro loads bitmaps with
    // premultiplied alpha, and the engine's blend states expect premultiplied colors, so
    // every color handed to Allegro (tints, primitives, clears, text) is premultiplied here.
    inline ALLEGRO_COLOR ToAllegroColor(const Color& color)
    {
        return al_premul_rgba_f(color.Red, color.Green, color.Blue, color.Alpha);
    }
}
