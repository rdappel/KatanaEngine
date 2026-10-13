#pragma once

#include "ParticleBehaviors.h"
#include "Core/Math/Circle.h"
#include "Core/Resource/Texture.h"

#include <memory>

namespace KatanaEngine
{
	namespace Particles
	{
		// Draws each SimpleParticle as a texture centred on its position, tinted by its Color
		// (alpha included) and rotated by its Rotation.
		//
		// Scale is the particle's size in pixels across the texture's longer side, so the same
		// settings look the same whatever the texture's resolution.
		class TextureParticleDrawer : public ParticleDrawer<SimpleParticle>
		{
		public:
			explicit TextureParticleDrawer(std::shared_ptr<Texture> texture)
				: m_texture(std::move(texture))
			{
				if (!m_texture) return;

				const float width = static_cast<float>(m_texture->GetWidth());
				const float height = static_cast<float>(m_texture->GetHeight());
				m_origin = Vector2(width * 0.5f, height * 0.5f);
				m_pixelsToScale = 1.0f / std::max(1.0f, std::max(width, height));
			}

		protected:
			void DrawParticle(const SimpleParticle& particle, SpriteBatch& spriteBatch) override
			{
				if (!m_texture || particle.Scale <= 0.0f || particle.Color.Alpha <= 0.0f) return;

				const float scale = particle.Scale * m_pixelsToScale;
				spriteBatch.Draw(m_texture, particle.Position, nullptr, particle.Color, particle.Rotation,
					m_origin, Vector2(scale, scale), SpriteEffects::NONE, 0.0f, false);
			}

		private:
			std::shared_ptr<Texture> m_texture;
			Vector2 m_origin;
			float m_pixelsToScale = 1.0f;
		};

		// Draws each SimpleParticle as a filled circle; Scale is the diameter in pixels.
		class CircleParticleDrawer : public ParticleDrawer<SimpleParticle>
		{
		protected:
			void DrawParticle(const SimpleParticle& particle, SpriteBatch& spriteBatch) override
			{
				if (particle.Scale <= 0.0f || particle.Color.Alpha <= 0.0f) return;

				spriteBatch.DrawPrimitive(Circle(particle.Position, particle.Scale * 0.5f), particle.Color, particle.Color, 0.0f);
			}
		};
	}
}
