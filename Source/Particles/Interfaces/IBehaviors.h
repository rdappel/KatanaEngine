#pragma once

#include "../Particle.h"
#include "Core/GameTime.h"
#include "Core/Graphics/SpriteBatch.h"

namespace KatanaEngine
{
	namespace Particles
	{
		class IParticleInitializer
		{
		public:
			virtual ~IParticleInitializer() = default;
			virtual void Initialize(Particle& particle, const Vector2& emitterPosition) = 0;
		};

		class IParticleUpdater
		{
		public:
			virtual ~IParticleUpdater() = default;
			virtual void Update(Particle& particle, const GameTime& gameTime) = 0;
		};

		class IParticleDrawer
		{
		public:
			virtual ~IParticleDrawer() = default;
			virtual void Draw(const Particle& particle, SpriteBatch& spriteBatch) = 0;
		};
	}
}