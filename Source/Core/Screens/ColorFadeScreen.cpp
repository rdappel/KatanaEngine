#include "pch.h"

#include "ColorFadeScreen.h"
#include "Core/Game.h"
#include "Core/Graphics/SpriteBatch.h"
#include "Core/Math/Math.h"
#include "Core/Math/Rectangle.h"
#include "Core/Resource/Effect.h"
#include "Core/Resource/ResourceManager.h"
#include "ScreenManager.h"

namespace KatanaEngine
{
	ColorFadeScreen::ColorFadeScreen(const Color& color, bool show)
		: Screen(show)
		, m_color(color)
	{ }

	void ColorFadeScreen::SetFadeInTime(float seconds)
	{
		m_fadeInTime = Math::Max(0.0f, seconds);
	}

	void ColorFadeScreen::SetFadeOutTime(float seconds)
	{
		m_fadeOutTime = Math::Max(0.0f, seconds);
	}

	bool ColorFadeScreen::IsFadingIn() const
	{
		return m_state == FadeState::FadingIn;
	}

	bool ColorFadeScreen::IsFadingOut() const
	{
		return m_state == FadeState::FadingOut;
	}

	float ColorFadeScreen::GetFadeInValue() const
	{
		return IsFadingIn() ? GetFadeProgress(m_fadeElapsed, m_fadeInTime) : 0.0f;
	}

	float ColorFadeScreen::GetFadeOutValue() const
	{
		return IsFadingOut() ? GetFadeProgress(m_fadeElapsed, m_fadeOutTime) : 0.0f;
	}

	void ColorFadeScreen::LoadContent(ResourceManager& resourceManager)
	{
		m_fadeEffect = resourceManager.Load<Effect>("Effects/ColorFade.keeffect");
		if (!m_fadeEffect)
		{
			std::cerr << "WARNING: ColorFadeScreen running without shader effect (not supported or failed to load)" << std::endl;
		}
	}

	void ColorFadeScreen::Update(const GameTime& gameTime)
	{
		if (m_state != FadeState::FadingIn && m_state != FadeState::FadingOut)
		{
			return;
		}

		m_fadeElapsed += static_cast<float>(gameTime.ElapsedTime);

		if (m_state == FadeState::FadingIn && GetFadeProgress(m_fadeElapsed, m_fadeInTime) >= 1.0f)
		{
			m_state = FadeState::Visible;
			m_fadeElapsed = 0.0f;
			return;
		}

		if (m_state == FadeState::FadingOut && GetFadeProgress(m_fadeElapsed, m_fadeOutTime) >= 1.0f)
		{
			RemoveFromManager();
		}
	}

	void ColorFadeScreen::Draw(SpriteBatch& spriteBatch)
	{
		if (!GetScreenManager())
		{
			return;
		}

		const float overlayAmount = GetOverlayAmount();
		if (overlayAmount <= 0.0f)
		{
			return;
		}

		const Game& game = GetScreenManager()->GetGame();
		Rectangle screenRect(0.0f, 0.0f,
			static_cast<float>(game.GetScreenWidth()),
			static_cast<float>(game.GetScreenHeight()));

		if (m_fadeEffect && m_fadeEffect->IsValid())
		{
			// Shader-based fade: use effect to apply color and fade amount
			m_fadeEffect->SetColor("FadeColor", m_color);
			m_fadeEffect->SetFloat("FadeAmount", overlayAmount);
			spriteBatch.Begin(SpriteSortMode::DEFERRED, BlendState::ALPHA_BLEND, m_fadeEffect);
			spriteBatch.DrawPrimitive(screenRect, Color::WHITE, Color::TRANSPARENT);
			spriteBatch.End();
		}
		else
		{
			// Fallback: Draw solid color rectangle with alpha blending
			spriteBatch.Begin(SpriteSortMode::DEFERRED, BlendState::ALPHA_BLEND);
			spriteBatch.DrawPrimitive(screenRect, m_color * overlayAmount, Color::TRANSPARENT);
			spriteBatch.End();
		}
	}

	void ColorFadeScreen::Show()
	{
		if (!BeginShow())
		{
			return;
		}

		m_state = FadeState::FadingIn;
		m_fadeElapsed = 0.0f;

		if (m_fadeInTime <= 0.0f)
		{
			m_state = FadeState::Visible;
		}
	}

	void ColorFadeScreen::Exit()
	{
		if (IsFadingOut())
		{
			return;
		}

		if (!BeginExit())
		{
			return;
		}

		m_state = FadeState::FadingOut;
		m_fadeElapsed = 0.0f;

		if (m_fadeOutTime <= 0.0f)
		{
			RemoveFromManager();
		}
	}

	float ColorFadeScreen::GetFadeProgress(float elapsed, float duration) const
	{
		if (duration <= 0.0f)
		{
			return 1.0f;
		}

		return Math::Clamp(elapsed / duration, 0.0f, 1.0f);
	}

	float ColorFadeScreen::GetOverlayAmount() const
	{
		switch (m_state)
		{
		case FadeState::Hidden:
			return 1.0f;
		case FadeState::FadingIn:
			return 1.0f - GetFadeInValue();
		case FadeState::Visible:
			return 0.0f;
		case FadeState::FadingOut:
			return GetFadeOutValue();
		default:
			return 0.0f;
		}
	}
}
