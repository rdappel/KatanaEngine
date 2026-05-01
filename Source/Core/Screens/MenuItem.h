#pragma once

#include "Core/Math/Region.h"

#include <functional>

namespace KatanaEngine
{
	struct GameTime;
	class SpriteBatch;

	class MenuItem
	{
	public:
		virtual ~MenuItem() = default;

		std::function<void(MenuItem&)> OnFocus;
		std::function<void(MenuItem&)> OnSelect;

		const Region& GetBounds() const { return m_bounds; }
		void SetBounds(const Region& bounds) { m_bounds = bounds; }

		bool IsEnabled() const { return m_isEnabled; }
		bool IsDisabled() const { return !m_isEnabled; }
		void SetEnabled(bool enabled);
		void SetDisabled(bool disabled) { SetEnabled(!disabled); }

		bool IsFocused() const { return m_isFocused; }
		void SetFocused(bool focused);
		void Select();

		virtual void Update(const GameTime& gameTime) {}
		virtual void Draw(SpriteBatch& spriteBatch) = 0;

	protected:
		MenuItem() = default;

	private:
		Region m_bounds = Region(0, 0, 200, 48);
		bool m_isFocused = false;
		bool m_isEnabled = true;
	};
}
