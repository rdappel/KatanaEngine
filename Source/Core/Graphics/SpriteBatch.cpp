#include "pch.h"

#include "SpriteBatch.h"
#include "Core/Resource/Effect.h"
#include "Core/Interfaces/IRenderContext.h"
#include "Core/Resource/Texture.h"
#include "Core/Platform/PlatformTexture.h"
#include "Core/Resource/Font.h"
#include "Core/Platform/PlatformFont.h"
#include "Core/Platform/PlatformColor.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <algorithm>
#include <cmath>
#include <type_traits>

namespace KatanaEngine
{
    SpriteBatch::SpriteBatch()
        : m_sortMode(SpriteSortMode::DEFERRED)
        , m_blendState(BlendState::ALPHA_BLEND)
        , m_hasBegun(false)
    { }

    SpriteBatch::~SpriteBatch()
    {
        if (m_hasBegun) End();
    }

    void SpriteBatch::Begin()
    {
        Begin(SpriteSortMode::DEFERRED, BlendState::ALPHA_BLEND, nullptr, Vector2::ZERO);
    }

    void SpriteBatch::Begin(SpriteSortMode sortMode)
    {
        Begin(sortMode, BlendState::ALPHA_BLEND, nullptr, Vector2::ZERO);
    }

    void SpriteBatch::Begin(SpriteSortMode sortMode, BlendState blendState)
    {
        Begin(sortMode, blendState, nullptr, Vector2::ZERO);
    }

    void SpriteBatch::Begin(SpriteSortMode sortMode, BlendState blendState, const std::shared_ptr<Effect>& effect)
    {
        Begin(sortMode, blendState, effect, static_cast<const IRenderContext*>(nullptr));
    }

    void SpriteBatch::Begin(SpriteSortMode sortMode, BlendState blendState, const std::shared_ptr<Effect>& effect, const IRenderContext* renderContext)
    {
        m_zoom = renderContext ? renderContext->GetZoom() : 1.0f;
        Begin(sortMode, blendState, effect, renderContext ? renderContext->GetViewOffset() : Vector2::ZERO);
    }

    void SpriteBatch::Begin(SpriteSortMode sortMode, BlendState blendState, const std::shared_ptr<Effect>& effect, const Vector2& viewOffset)
    {
        m_hasBegun = true;
        m_sortMode = sortMode;
        m_blendState = blendState;
        m_effect = effect;
        m_viewOffset = viewOffset;
        m_previousShader = al_get_current_shader();
        m_spriteQueue.clear();
        m_textQueue.clear();
        m_primitiveQueue.clear();

        ApplyBlendState(blendState);

        if (m_effect && m_effect->IsValid())
        {
            m_effect->Apply();
        }

        if (m_zoom != 1.0f)
        {
            al_copy_transform(reinterpret_cast<ALLEGRO_TRANSFORM*>(m_savedTransform),
                              al_get_current_transform());
            ALLEGRO_TRANSFORM zoomT;
            al_identity_transform(&zoomT);
            al_scale_transform(&zoomT, m_zoom, m_zoom);
            al_use_transform(&zoomT);
        }

        if (sortMode != SpriteSortMode::IMMEDIATE)
        {
            al_hold_bitmap_drawing(true);
        }
    }

    void SpriteBatch::End()
    {
        if (!m_hasBegun) return;

        FlushBatch();

        if (m_sortMode != SpriteSortMode::IMMEDIATE)
        {
            al_hold_bitmap_drawing(false);
        }

        RestoreBlendState();
        al_use_shader(m_previousShader);
        m_previousShader = nullptr;
        m_effect.reset();
        m_viewOffset = Vector2::ZERO;

        if (m_zoom != 1.0f)
        {
            al_use_transform(reinterpret_cast<const ALLEGRO_TRANSFORM*>(m_savedTransform));
            m_zoom = 1.0f;
        }

        m_hasBegun = false;
    }

    void SpriteBatch::ApplyBlendState(BlendState blendState)
    {
        switch (blendState)
        {
        case BlendState::ALPHA_BLEND:
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
            break;
        case BlendState::ADDITIVE:
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_ONE);
            break;
        case BlendState::NON_PREMULTIPLIED:
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
            break;
        case BlendState::OPAQUE:
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
            break;
        }
    }

    void SpriteBatch::RestoreBlendState()
    {
        // Restore default alpha blending
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    }

    void SpriteBatch::FlushBatch()
    {
        // Sort sprites based on sort mode
        if (!m_spriteQueue.empty())
        {
            switch (m_sortMode)
            {
            case SpriteSortMode::TEXTURE:
                std::stable_sort(m_spriteQueue.begin(), m_spriteQueue.end(),
                    [](const SpriteInfo& a, const SpriteInfo& b) {
                        return a.texture.get() < b.texture.get();
                    });
                break;

            case SpriteSortMode::BACK_TO_FRONT:
                std::stable_sort(m_spriteQueue.begin(), m_spriteQueue.end(),
                    [](const SpriteInfo& a, const SpriteInfo& b) {
                        return a.layerDepth > b.layerDepth;
                    });
                break;

            case SpriteSortMode::FRONT_TO_BACK:
                std::stable_sort(m_spriteQueue.begin(), m_spriteQueue.end(),
                    [](const SpriteInfo& a, const SpriteInfo& b) {
                        return a.layerDepth < b.layerDepth;
                    });
                break;

            case SpriteSortMode::DEFERRED:
            case SpriteSortMode::IMMEDIATE:
            default:
                // No sorting needed
                break;
            }

            // Draw all sprites
            for (const auto& sprite : m_spriteQueue)
            {
                DrawSprite(sprite);
            }

            m_spriteQueue.clear();
        }

        // Sort text based on sort mode (BackToFront/FrontToBack only)
        if (!m_textQueue.empty())
        {
            switch (m_sortMode)
            {
            case SpriteSortMode::BACK_TO_FRONT:
                std::stable_sort(m_textQueue.begin(), m_textQueue.end(),
                    [](const TextInfo& a, const TextInfo& b) {
                        return a.layerDepth > b.layerDepth;
                    });
                break;

            case SpriteSortMode::FRONT_TO_BACK:
                std::stable_sort(m_textQueue.begin(), m_textQueue.end(),
                    [](const TextInfo& a, const TextInfo& b) {
                        return a.layerDepth < b.layerDepth;
                    });
                break;

            default:
                break;
            }

            // Draw all text
            for (const auto& text : m_textQueue)
            {
                DrawText(text);
            }

            m_textQueue.clear();
        }

        // Sort and draw primitives
        if (!m_primitiveQueue.empty())
        {
            switch (m_sortMode)
            {
            case SpriteSortMode::BACK_TO_FRONT:
                std::stable_sort(m_primitiveQueue.begin(), m_primitiveQueue.end(),
                    [](const PrimitiveEntry& a, const PrimitiveEntry& b) {
                        return a.layerDepth > b.layerDepth;
                    });
                break;

            case SpriteSortMode::FRONT_TO_BACK:
                std::stable_sort(m_primitiveQueue.begin(), m_primitiveQueue.end(),
                    [](const PrimitiveEntry& a, const PrimitiveEntry& b) {
                        return a.layerDepth < b.layerDepth;
                    });
                break;

            default:
                break;
            }

            // Draw all primitives
            for (const auto& entry : m_primitiveQueue)
            {
                std::visit([this](const auto& e) {
                    ALLEGRO_COLOR fill = ToAllegroColor(Color::TRANSPARENT);
                    ALLEGRO_COLOR stroke = ToAllegroColor(Color::TRANSPARENT);

                    if constexpr (std::is_same_v<std::decay_t<decltype(e)>, LineEntry>)
                    {
                        Line shape = e.shape;
                        shape.Translate(Vector2(-m_viewOffset.X, -m_viewOffset.Y));
                        stroke = ToAllegroColor(e.strokeColor);
                        shape.Draw(fill, stroke, e.strokeWidth, 0, 0);
                    }
                    else if constexpr (std::is_same_v<std::decay_t<decltype(e)>, CircleEntry>)
                    {
                        Circle shape = e.shape;
                        shape.Translate(Vector2(-m_viewOffset.X, -m_viewOffset.Y));
                        fill = ToAllegroColor(e.fillColor);
                        stroke = ToAllegroColor(e.strokeColor);
                        shape.Draw(fill, stroke, e.strokeWidth, 0, 0);
                    }
                    else if constexpr (std::is_same_v<std::decay_t<decltype(e)>, RectangleEntry>)
                    {
                        Rectangle shape = e.shape;
                        shape.Translate(Vector2(-m_viewOffset.X, -m_viewOffset.Y));
                        fill = ToAllegroColor(e.fillColor);
                        stroke = ToAllegroColor(e.strokeColor);
                        shape.Draw(fill, stroke, e.strokeWidth, 0, 0);
                    }
                    else if constexpr (std::is_same_v<std::decay_t<decltype(e)>, SplineEntry>)
                    {
                        Spline shape = e.shape;
                        shape.Translate(Vector2(-m_viewOffset.X, -m_viewOffset.Y));
                        stroke = ToAllegroColor(e.strokeColor);
                        shape.Draw(fill, stroke, e.strokeWidth, 0, 0);
                    }
                }, entry.primitive);
            }

            m_primitiveQueue.clear();
        }
    }

    void SpriteBatch::DrawSprite(const SpriteInfo& sprite) const
    {
        if (!sprite.texture || !sprite.texture->IsValid()) return;

        ALLEGRO_BITMAP* bitmap = sprite.texture->GetBitmap();

        float sx, sy, sw, sh;
        if (sprite.hasSourceRegion)
        {
            sx = static_cast<float>(sprite.sourceRegion.X);
            sy = static_cast<float>(sprite.sourceRegion.Y);
            sw = static_cast<float>(sprite.sourceRegion.Width);
            sh = static_cast<float>(sprite.sourceRegion.Height);
        }
        else
        {
            sx = 0;
            sy = 0;
            sw = static_cast<float>(sprite.texture->GetWidth());
            sh = static_cast<float>(sprite.texture->GetHeight());
        }

        int flags = 0;
        if (static_cast<int>(sprite.effects) & static_cast<int>(SpriteEffects::FLIP_HORIZONTALLY))
            flags |= ALLEGRO_FLIP_HORIZONTAL;
        if (static_cast<int>(sprite.effects) & static_cast<int>(SpriteEffects::FLIP_VERTICALLY))
            flags |= ALLEGRO_FLIP_VERTICAL;

        Vector2 drawPosition = sprite.position - m_viewOffset;
        if (sprite.pixelSnap)
        {
            drawPosition.X = std::roundf(drawPosition.X);
            drawPosition.Y = std::roundf(drawPosition.Y);
        }

        al_draw_tinted_scaled_rotated_bitmap_region(bitmap,
            sx, sy, sw, sh,
            ToAllegroColor(sprite.tint),
            sprite.origin.X, sprite.origin.Y,
            drawPosition.X, drawPosition.Y,
            sprite.scale.X, sprite.scale.Y,
            sprite.rotation,
            flags);
    }

    void SpriteBatch::Draw(const std::shared_ptr<Texture>& texture, const Vector2& position)
    {
        Draw(texture, position, Color::WHITE);
    }

    void SpriteBatch::Draw(const std::shared_ptr<Texture>& texture, const Vector2& position, const Color& tint)
    {
        Draw(texture, position, nullptr, tint, 0.0f, Vector2::ZERO, Vector2::ONE, SpriteEffects::NONE, 0.0f);
    }

    void SpriteBatch::Draw(const std::shared_ptr<Texture>& texture, const Vector2& position,
        const Region& sourceRegion, const Color& tint)
    {
        Draw(texture, position, &sourceRegion, tint, 0.0f, Vector2::ZERO, Vector2::ONE, SpriteEffects::NONE, 0.0f);
    }

    void SpriteBatch::Draw(const std::shared_ptr<Texture>& texture, const Vector2& position,
        const Region& sourceRegion, const Color& tint, bool pixelSnap)
    {
        Draw(texture, position, &sourceRegion, tint, 0.0f, Vector2::ZERO, Vector2::ONE, SpriteEffects::NONE, 0.0f, pixelSnap);
    }

    void SpriteBatch::Draw(const std::shared_ptr<Texture>& texture, const Region& destinationRegion, const Color& tint)
    {
        if (!texture || !texture->IsValid()) return;

        Vector2 scale(
            static_cast<float>(destinationRegion.Width) / texture->GetWidth(),
            static_cast<float>(destinationRegion.Height) / texture->GetHeight()
        );

        Draw(texture, Vector2(static_cast<float>(destinationRegion.X), static_cast<float>(destinationRegion.Y)),
            nullptr, tint, 0.0f, Vector2::ZERO, scale, SpriteEffects::NONE, 0.0f);
    }

    void SpriteBatch::Draw(const std::shared_ptr<Texture>& texture, const Region& destinationRegion,
        const Region& sourceRegion, const Color& tint)
    {
        Vector2 scale(
            static_cast<float>(destinationRegion.Width) / sourceRegion.Width,
            static_cast<float>(destinationRegion.Height) / sourceRegion.Height
        );

        Draw(texture, Vector2(static_cast<float>(destinationRegion.X), static_cast<float>(destinationRegion.Y)),
            &sourceRegion, tint, 0.0f, Vector2::ZERO, scale, SpriteEffects::NONE, 0.0f);
    }

    void SpriteBatch::Draw(const std::shared_ptr<Texture>& texture, const Vector2& position,
        const Region* sourceRegion, const Color& tint, float rotation,
        const Vector2& origin, float scale, SpriteEffects effects, float layerDepth)
    {
        Draw(texture, position, sourceRegion, tint, rotation, origin, Vector2(scale, scale), effects, layerDepth);
    }

    void SpriteBatch::Draw(const std::shared_ptr<Texture>& texture, const Vector2& position,
        const Region* sourceRegion, const Color& tint, float rotation,
        const Vector2& origin, const Vector2& scale, SpriteEffects effects, float layerDepth, bool pixelSnap)
    {
        if (!texture || !texture->IsValid()) return;

        SpriteInfo sprite;
        sprite.texture = texture;
        sprite.position = position;
        sprite.hasSourceRegion = (sourceRegion != nullptr);
        if (sourceRegion)
        {
            sprite.sourceRegion = *sourceRegion;
        }
        sprite.tint = tint;
        sprite.rotation = rotation;
        sprite.origin = origin;
        sprite.scale = scale;
        sprite.effects = effects;
        sprite.layerDepth = layerDepth;
        sprite.pixelSnap = pixelSnap;

        if (m_sortMode == SpriteSortMode::IMMEDIATE)
        {
            DrawSprite(sprite);
        }
        else
        {
            m_spriteQueue.push_back(sprite);
        }
    }

    void SpriteBatch::DrawText(const TextInfo& textInfo) const
    {
        if (!textInfo.font || !textInfo.font->IsValid()) return;

        ALLEGRO_FONT* font = textInfo.font->m_pData->pFont;
        ALLEGRO_COLOR color = ToAllegroColor(textInfo.color);

        if (textInfo.rotation == 0.0f && textInfo.scale.X == 1.0f && textInfo.scale.Y == 1.0f)
        {
            // Simple case: no rotation or scaling
            al_draw_text(font, color,
                static_cast<int>(std::lround(textInfo.position.X - m_viewOffset.X - textInfo.origin.X)),
                static_cast<int>(std::lround(textInfo.position.Y - m_viewOffset.Y - textInfo.origin.Y)),
                0, textInfo.text.c_str());
        }
        else
        {
            // Complex case: use transformation for rotation and scaling
            ALLEGRO_TRANSFORM transform, prevTransform;
            al_copy_transform(&prevTransform, al_get_current_transform());

            al_identity_transform(&transform);
            al_translate_transform(&transform, -textInfo.origin.X, -textInfo.origin.Y);
            al_scale_transform(&transform, textInfo.scale.X, textInfo.scale.Y);
            al_rotate_transform(&transform, textInfo.rotation);
            al_translate_transform(&transform, textInfo.position.X - m_viewOffset.X, textInfo.position.Y - m_viewOffset.Y);
            al_use_transform(&transform);

            al_draw_text(font, color, 0, 0, 0, textInfo.text.c_str());

            al_use_transform(&prevTransform);
        }
    }

    void SpriteBatch::DrawString(const std::shared_ptr<Font>& font, const std::string& text, const Vector2& position)
    {
        DrawString(font, text, position, Color::WHITE, 0.0f, Vector2::ZERO, Vector2::ONE, 0.0f);
    }

    void SpriteBatch::DrawString(const std::shared_ptr<Font>& font, const std::string& text, const Vector2& position,
        const Color& color)
    {
        DrawString(font, text, position, color, 0.0f, Vector2::ZERO, Vector2::ONE, 0.0f);
    }

    void SpriteBatch::DrawString(const std::shared_ptr<Font>& font, const std::string& text, const Vector2& position,
        const Color& color, float rotation, const Vector2& origin, float scale,
        float layerDepth)
    {
        DrawString(font, text, position, color, rotation, origin, Vector2(scale, scale), layerDepth);
    }

    void SpriteBatch::DrawString(const std::shared_ptr<Font>& font, const std::string& text, const Vector2& position,
        const Color& color, float rotation, const Vector2& origin, const Vector2& scale,
        float layerDepth)
    {
        if (!font || !font->IsValid()) return;

        TextInfo textInfo;
        textInfo.font = font;
        textInfo.text = text;
        textInfo.position = position;
        textInfo.color = color;
        textInfo.rotation = rotation;
        textInfo.origin = origin;
        textInfo.scale = scale;
        textInfo.layerDepth = layerDepth;

        if (m_sortMode == SpriteSortMode::IMMEDIATE)
        {
            DrawText(textInfo);
        }
        else
        {
            m_textQueue.push_back(textInfo);
        }
    }

    // Primitive drawing implementations

    void SpriteBatch::DrawPrimitive(const Line& line, const Color& strokeColor, float strokeWidth, float layerDepth)
    {
        LineEntry entry{ line, strokeColor, strokeWidth };
        if (m_sortMode == SpriteSortMode::IMMEDIATE)
        {
            ALLEGRO_COLOR fill = al_map_rgba(0, 0, 0, 0);
            ALLEGRO_COLOR stroke = ToAllegroColor(strokeColor);
            Line drawLine = line;
            drawLine.Translate(Vector2(-m_viewOffset.X, -m_viewOffset.Y));
            drawLine.Draw(fill, stroke, strokeWidth, 0, 0);
        }
        else
        {
            m_primitiveQueue.push_back({ entry, layerDepth });
        }
    }

    void SpriteBatch::DrawPrimitive(const Circle& circle, const Color& fillColor, const Color& strokeColor, float strokeWidth, float layerDepth)
    {
        CircleEntry entry{ circle, fillColor, strokeColor, strokeWidth };
        if (m_sortMode == SpriteSortMode::IMMEDIATE)
        {
            ALLEGRO_COLOR fill = ToAllegroColor(fillColor);
            ALLEGRO_COLOR stroke = ToAllegroColor(strokeColor);
            Circle drawCircle = circle;
            drawCircle.Translate(Vector2(-m_viewOffset.X, -m_viewOffset.Y));
            drawCircle.Draw(fill, stroke, strokeWidth, 0, 0);
        }
        else
        {
            m_primitiveQueue.push_back({ entry, layerDepth });
        }
    }

    void SpriteBatch::DrawPrimitive(const Rectangle& rectangle, const Color& fillColor, const Color& strokeColor, float strokeWidth, float layerDepth)
    {
        RectangleEntry entry{ rectangle, fillColor, strokeColor, strokeWidth };
        if (m_sortMode == SpriteSortMode::IMMEDIATE)
        {
            ALLEGRO_COLOR fill = ToAllegroColor(fillColor);
            ALLEGRO_COLOR stroke = ToAllegroColor(strokeColor);
            Rectangle drawRectangle = rectangle;
            drawRectangle.Translate(Vector2(-m_viewOffset.X, -m_viewOffset.Y));
            drawRectangle.Draw(fill, stroke, strokeWidth, 0, 0);
        }
        else
        {
            m_primitiveQueue.push_back({ entry, layerDepth });
        }
    }

    void SpriteBatch::DrawPrimitive(const Spline& spline, const Color& strokeColor, float strokeWidth, float layerDepth)
    {
        SplineEntry entry{ spline, strokeColor, strokeWidth };
        if (m_sortMode == SpriteSortMode::IMMEDIATE)
        {
            ALLEGRO_COLOR fill = al_map_rgba(0, 0, 0, 0);
            ALLEGRO_COLOR stroke = ToAllegroColor(strokeColor);
            Spline drawSpline = spline;
            drawSpline.Translate(Vector2(-m_viewOffset.X, -m_viewOffset.Y));
            drawSpline.Draw(fill, stroke, strokeWidth, 0, 0);
        }
        else
        {
            m_primitiveQueue.push_back({ entry, layerDepth });
        }
    }
}
