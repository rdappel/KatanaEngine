#pragma once

#include "Screen.h"
#include "Core/Graphics/Color.h"

#include <memory>

namespace KatanaEngine
{
	class Effect;

	class ColorFadeScreen : public Screen
	{
	public:
		ColorFadeScreen(const Color& color = Color::BLACK, bool show = true);
		virtual ~ColorFadeScreen() = default;

		void SetFadeInTime(float seconds);
		void SetFadeOutTime(float seconds);
		void SetColor(const Color& color) { m_color = color; }

		bool IsFadingIn() const;
		bool IsFadingOut() const;
		float GetFadeInValue() const;
		float GetFadeOutValue() const;

		void LoadContent(ResourceManager& resourceManager) override;
		void Update(const GameTime& gameTime) override;
		void Draw(SpriteBatch& spriteBatch) override;
		void Show() override;
		void Exit() override;

		void ShowImmediately()
		{
			m_state = FadeState::Visible;
			m_fadeElapsed = 0.0f;
		}
		void ExitImmediately() { RemoveFromManager(); }

	private:
		enum class FadeState
		{
			Hidden,
			FadingIn,
			Visible,
			FadingOut
		};

		float GetFadeProgress(float elapsed, float duration) const;
		float GetOverlayAmount() const;

		Color m_color;
		float m_fadeInTime = 1.0f;
		float m_fadeOutTime = 1.0f;
		float m_fadeElapsed = 0.0f;
		FadeState m_state = FadeState::Hidden;
		std::shared_ptr<Effect> m_fadeEffect;
	};
}
