#pragma once

#include "Core/Game.h"
#include "Core/GameTime.h"
#include <functional>

namespace KatanaEngine
{
	namespace GUI
	{
		class Control
		{
			using FocusChangedCallback = std::function<void(Control*, bool)>;

		public:
			Control();
			virtual ~Control() = default;

			virtual void HandleInput(const InputState& input) = 0;
			virtual void Update(const GameTime& gameTime) = 0;
			virtual void Draw(SpriteBatch& spriteBatch) = 0;

			Vector2 GetPosition() const { return m_position; }
			void SetPosition(const Vector2& position) { m_position = position; }
			void SetPosition(float x, float y) { m_position = Vector2(x, y); }

			Vector2 GetSize() const { return m_size; }
			void SetSize(const Vector2& size) { m_size = size; }
			void SetSize(float width, float height) { m_size = Vector2(width, height); }

			Region GetBounds() const;

			bool IsEnabled() const { return m_isEnabled; }
			void SetEnabled(bool enabled) { m_isEnabled = enabled; }

			bool IsVisible() const { return m_isVisible; }
			void SetVisible(bool visible) { m_isVisible = visible; }

			bool IsFocused() const { return m_isFocused; }
			void SetFocused(bool focused);

			int GetTabIndex() const { return m_tabIndex; }
			void SetTabIndex(int index) { m_tabIndex = index; }

			std::shared_ptr<Font> GetFont() const { return m_font; }
			void SetFont(std::shared_ptr<Font> font) { m_font = font; }

			Color GetBackgroundColor() const { return m_backgroundColor; }
			void SetBackgroundColor(const Color& color) { m_backgroundColor = color; }

			Color GetForegroundColor() const { return m_foregroundColor; }
			void SetForegroundColor(const Color& color) { m_foregroundColor = color; }

			Color GetBorderColor() const { return m_borderColor; }
			void SetBorderColor(const Color& color) { m_borderColor = color; }
			void SetOnFocusChanged(FocusChangedCallback callback) { m_onFocusChanged = callback; }

		protected:
			virtual void OnFocusChanged(bool focused);

			Vector2 m_position;
			Vector2 m_size;
			bool m_isEnabled = true;
			bool m_isVisible = true;
			bool m_isFocused = false;
			int m_tabIndex = 0;

			std::shared_ptr<Font> m_font;
			Color m_backgroundColor;
			Color m_foregroundColor;
			Color m_borderColor;

			FocusChangedCallback m_onFocusChanged;
		};
	}
}
