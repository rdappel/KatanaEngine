#pragma once

#include "Interfaces\IBehaviors.h"

#include <vector>
#include <memory>

namespace KatanaEngine
{
	namespace Particles
	{
		struct EmitterConfig
		{
			int MaxParticles = 100;
			float EmissionRate = 10.0f;		// Particles per second
			bool IsLooping = true;
			float Duration = 1.0f;			// For non-looping emitters

			EmitterConfig() = default;
		};

		template<typename TParticle>
		class ParticleEmitter
		{
			static_assert(std::is_base_of<Particle, TParticle>::value,
				"TParticle must derive from Particle");

		public:
			
			ParticleEmitter(const EmitterConfig& config = EmitterConfig())
				: m_config(config)
				, m_position(Vector2::ZERO)
				, m_emissionAccumulator(0.0f)
				, m_age(0.0f)
				, m_isEmitting(true)
			{
				m_particles.resize(config.MaxParticles);
			}

			void SetPosition(const Vector2& position) { m_position = position; }
			const Vector2& GetPosition() const { return m_position; }

			void Start() { m_isEmitting = true; m_age = 0.0f; }
			void Stop() { m_isEmitting = false; }
			bool IsEmitting() const { return m_isEmitting; }

			void AddInitializer(std::shared_ptr<IParticleInitializer> initializer)
			{
				m_initializers.push_back(initializer);
			}

			void AddUpdater(std::shared_ptr<IParticleUpdater> updater)
			{
				m_updaters.push_back(updater);
			}

			void SetDrawer(std::shared_ptr<IParticleDrawer> drawer)
			{
				m_drawer = drawer;
			}

			void Update(const GameTime& gameTime)
			{
				float deltaTime = static_cast<float>(gameTime.ElapsedTime);
				m_age += deltaTime;
				if (!m_config.IsLooping && m_age >= m_config.Duration)
				{
					m_isEmitting = false;
				}

				if (m_isEmitting) EmitParticles(deltaTime);

				for (auto& particle : m_particles)
				{
					if (!particle.IsAlive) continue;

					for (auto& updater : m_updaters)
					{
						updater->Update(particle, gameTime);
					}
				}
			}

			void Draw(SpriteBatch& spriteBatch)
			{
				if (!m_drawer) return;

				for (const auto& particle : m_particles)
				{
					if (!particle.IsAlive) continue;

					m_drawer->Draw(particle, spriteBatch);
				}
			}

			int GetActiveParticleCount() const
			{
				int count = 0;
				for (const auto& p : m_particles)
				{
					if (p.IsAlive) ++count;
				}
				return count;
			}

			int GetMaxParticles() const { return m_config.MaxParticles; }

			void Emit(int count = 1)
			{
				for (int i = 0; i < count; ++i)
				{
					TParticle* particle = FindDeadParticle();
					if (particle)
					{
						InitializeParticle(*particle);
					}
				}
			}

			void Burst(int count) { Emit(count); }

			void Clear()
			{
				for (auto& particle : m_particles)
				{
					particle.IsAlive = false;
				}
			}

		private:

			void EmitParticles(float deltaTime)
			{
				m_emissionAccumulator += m_config.EmissionRate * deltaTime;

				while (m_emissionAccumulator >= 1.0f)
				{
					TParticle* particle = FindDeadParticle();
					if (particle) InitializeParticle(*particle);
					m_emissionAccumulator -= 1.0f;
				}
			}

			TParticle* FindDeadParticle()
			{
				for (auto& particle : m_particles)
				{
					if (!particle.IsAlive) return &particle;
				}
				return nullptr;
			}

			void InitializeParticle(TParticle& particle)
			{
				particle = TParticle();  // Reset to defaults
				particle.IsAlive = true;

				for (auto& initializer : m_initializers)
				{
					initializer->Initialize(particle, m_position);
				}
			}

			EmitterConfig m_config;
			Vector2 m_position;

			std::vector<TParticle> m_particles;

			// todo: do these need to be vectors?
			std::vector<std::shared_ptr<IParticleInitializer>> m_initializers;
			std::vector<std::shared_ptr<IParticleUpdater>> m_updaters;

			std::shared_ptr<IParticleDrawer> m_drawer;

			float m_emissionAccumulator;
			float m_age;
			bool m_isEmitting;
		};
	}
}
