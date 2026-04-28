#pragma once

#include "Core/Math/Vector2.h"
#include "Core/Math/Region.h"
#include "Core/Graphics/Color.h"
#include "Core/Resource/Texture.h"

namespace KatanaEngine
{
	namespace Particles
	{
		struct Particle
		{
			Vector2 Position;
			bool IsAlive = false;

			virtual ~Particle() = default;
		};

		struct TimedParticle : Particle
		{
			float Age = 0.0f;
			float Lifetime = 1.0f;

			float GetNormalizedAge() const { return Age / Lifetime; }
			bool IsExpired() const { return Age >= Lifetime; }
		};

		struct SimpleParticle : TimedParticle
		{
			Vector2 Velocity;
			Color Color = Color::WHITE;
			float Scale = 1.0f;
			float Rotation = 0.0f;
		};
	}
}