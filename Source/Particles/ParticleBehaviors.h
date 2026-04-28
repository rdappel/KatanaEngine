#pragma once

#include "Interfaces/IBehaviors.h"

namespace KatanaEngine
{
	namespace Particles
	{
		template<typename TParticle>
		class ParticleInitializer : public IParticleInitializer
		{
			static_assert(std::is_base_of<Particle, TParticle>::value,
				"TParticle must derive from Particle");

		public:
			void Initialize(Particle& particle, const Vector2& emitterPosition) final
			{
				InitializeParticle(static_cast<TParticle&>(particle), emitterPosition);
			}

		protected:
			virtual void InitializeParticle(TParticle& particle, const Vector2& emitterPosition) = 0;
		};

		template<typename TParticle>
		class ParticleUpdater : public IParticleUpdater
		{
			static_assert(std::is_base_of<Particle, TParticle>::value,
				"TParticle must derive from Particle");

		public:
			void Update(Particle& particle, const GameTime& gameTime) final
			{
				UpdateParticle(static_cast<TParticle&>(particle), gameTime);
			}

		protected:
			virtual void UpdateParticle(TParticle& particle, const GameTime& gameTime) = 0;
		};

		template<typename TParticle>
		class ParticleDrawer : public IParticleDrawer
		{
			static_assert(std::is_base_of<Particle, TParticle>::value,
				"TParticle must derive from Particle");

		public:
			void Draw(const Particle& particle, SpriteBatch& spriteBatch) final
			{
				DrawParticle(static_cast<const TParticle&>(particle), spriteBatch);
			}

		protected:
			virtual void DrawParticle(const TParticle& particle, SpriteBatch& spriteBatch) = 0;
		};
	}
}