#pragma once

#include "Core/Math/Vector2.h"
#include "Core/Math/Region.h"
#include "Core/Math/Line.h"
#include "Core/Math/Circle.h"
#include "Core/Math/Rectangle.h"
#include "Core/Math/Spline.h"
#include "Color.h"

#include <memory>
#include <string>
#include <variant>
#include <vector>

struct ALLEGRO_SHADER;

namespace KatanaEngine
{
    class Texture;
    class Font;
    class Effect;
    class IRenderContext;

    enum class SpriteEffects
    {
        NONE = 0,
        FLIP_HORIZONTALLY = 1,
        FLIP_VERTICALLY = 2
    };

    enum class SpriteSortMode
    {
        DEFERRED,       // Sprites are drawn when End() is called, in order submitted
        IMMEDIATE,      // Sprites are drawn immediately, no batching
        TEXTURE,        // Sprites are sorted by texture to minimize texture switches
        BACK_TO_FRONT,  // Sprites are sorted by layer depth, back to front (depth 1 drawn first)
        FRONT_TO_BACK   // Sprites are sorted by layer depth, front to back (depth 0 drawn first)
    };

    enum class BlendState
    {
        ALPHA_BLEND,        // Standard alpha blending (default)
        ADDITIVE,           // Additive blending (good for glow, particles)
        NON_PREMULTIPLIED,  // Alpha blend for non-premultiplied textures
        OPAQUE              // No blending, overwrites destination
    };

    class SpriteBatch
    {
    public:
        SpriteBatch();
        ~SpriteBatch();

        SpriteBatch(const SpriteBatch&) = delete;
        SpriteBatch& operator=(const SpriteBatch&) = delete;

        void Begin();
        void Begin(SpriteSortMode sortMode);
        void Begin(SpriteSortMode sortMode, BlendState blendState);
        void Begin(SpriteSortMode sortMode, BlendState blendState, const std::shared_ptr<Effect>& effect);
        void Begin(SpriteSortMode sortMode, BlendState blendState, const std::shared_ptr<Effect>& effect, const IRenderContext* renderContext);
        void Begin(SpriteSortMode sortMode, BlendState blendState, const std::shared_ptr<Effect>& effect, const Vector2& viewOffset);
        void End();

        // Draw entire texture at position
        void Draw(const std::shared_ptr<Texture>& texture, const Vector2& position);
        void Draw(const std::shared_ptr<Texture>& texture, const Vector2& position, const Color& tint);

        // Draw portion of texture (source region) at position
        void Draw(const std::shared_ptr<Texture>& texture, const Vector2& position,
            const Region& sourceRegion, const Color& tint);

        // Draw to destination region
        void Draw(const std::shared_ptr<Texture>& texture, const Region& destinationRegion, const Color& tint);
        void Draw(const std::shared_ptr<Texture>& texture, const Region& destinationRegion,
            const Region& sourceRegion, const Color& tint);

        // Full-featured draw with rotation, origin, scale, effects, and layer depth
        void Draw(const std::shared_ptr<Texture>& texture, const Vector2& position,
            const Region* sourceRegion, const Color& tint, float rotation,
            const Vector2& origin, float scale, SpriteEffects effects = SpriteEffects::NONE,
            float layerDepth = 0.0f);

        void Draw(const std::shared_ptr<Texture>& texture, const Vector2& position,
            const Region* sourceRegion, const Color& tint, float rotation,
            const Vector2& origin, const Vector2& scale, SpriteEffects effects = SpriteEffects::NONE,
            float layerDepth = 0.0f);

        // Text drawing
        void DrawString(const std::shared_ptr<Font>& font, const std::string& text, const Vector2& position);
        void DrawString(const std::shared_ptr<Font>& font, const std::string& text, const Vector2& position,
            const Color& color);
        void DrawString(const std::shared_ptr<Font>& font, const std::string& text, const Vector2& position,
            const Color& color, float rotation, const Vector2& origin, float scale,
            float layerDepth = 0.0f);
        void DrawString(const std::shared_ptr<Font>& font, const std::string& text, const Vector2& position,
            const Color& color, float rotation, const Vector2& origin, const Vector2& scale,
            float layerDepth = 0.0f);

        // Primitive drawing
        void DrawPrimitive(const Line& line, const Color& strokeColor, float strokeWidth = 1.0f, float layerDepth = 0.0f);
        void DrawPrimitive(const Circle& circle, const Color& fillColor, const Color& strokeColor, float strokeWidth = 1.0f, float layerDepth = 0.0f);
        void DrawPrimitive(const Rectangle& rectangle, const Color& fillColor, const Color& strokeColor, float strokeWidth = 1.0f, float layerDepth = 0.0f);
        void DrawPrimitive(const Spline& spline, const Color& strokeColor, float strokeWidth = 1.0f, float layerDepth = 0.0f);

    private:

        struct SpriteInfo
        {
            std::shared_ptr<Texture> texture;
            Vector2 position;
            Region sourceRegion;
            bool hasSourceRegion = false;
            Color tint;
            float rotation = 0.0f;
            Vector2 origin;
            Vector2 scale = Vector2::ONE;
            SpriteEffects effects = SpriteEffects::NONE;
            float layerDepth = 0.0f;
        };

        struct TextInfo
        {
            std::shared_ptr<Font> font;
            std::string text;
            Vector2 position;
            Color color;
            float rotation = 0.0f;
            Vector2 origin;
            Vector2 scale = Vector2::ONE;
            float layerDepth = 0.0f;
        };

        void DrawSprite(const SpriteInfo& sprite) const;
        void DrawText(const TextInfo& text) const;
        void FlushBatch();
        void ApplyBlendState(BlendState blendState);
        void RestoreBlendState();

        struct LineEntry { Line shape; Color strokeColor; float strokeWidth; };
        struct CircleEntry { Circle shape; Color fillColor; Color strokeColor; float strokeWidth; };
        struct RectangleEntry { Rectangle shape; Color fillColor; Color strokeColor; float strokeWidth; };
        struct SplineEntry { Spline shape; Color strokeColor; float strokeWidth; };

        struct PrimitiveEntry
        {
            std::variant<LineEntry, CircleEntry, RectangleEntry, SplineEntry> primitive;
            float layerDepth = 0.0f;
        };

        std::vector<SpriteInfo> m_spriteQueue;
        std::vector<TextInfo> m_textQueue;
        std::vector<PrimitiveEntry> m_primitiveQueue;
        SpriteSortMode m_sortMode;
        BlendState m_blendState;
        std::shared_ptr<Effect> m_effect;
        ALLEGRO_SHADER* m_previousShader = nullptr;
        Vector2 m_viewOffset = Vector2::ZERO;
        float m_zoom = 1.0f;
        float m_savedTransform[16] = {}; // stores ALLEGRO_TRANSFORM when zoom != 1
        bool m_hasBegun;
    };
}
