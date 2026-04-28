#pragma once

#include "Control.h"

namespace KatanaEngine
{
	namespace GUI
	{
		class Button : public Control
		{
			using ClickCallback = std::function<void(Button*)>;

		public:
			Button();
			Button(const std::string& text);
			virtual ~Button() = default;

			void HandleInput(const InputState& input) override;
			void Update(const GameTime& gameTime) override;
			void Draw(SpriteBatch& spriteBatch) override;

			const std::string& GetText() const { return m_text; }
			void SetText(const std::string& text) { m_text = text; }

			void SetHoverColor(const Color& color) { m_hoverColor = color; }
			void SetPressedColor(const Color& color) { m_pressedColor = color; }
			void SetDisabledColor(const Color& color) { m_disabledColor = color; }
			void SetFocusedBorderColor(const Color& color) { m_focusedBorderColor = color; }

			void SetOnClick(ClickCallback callback) { m_onClick = callback; }

			bool IsPressed() const { return m_isPressed; }
			bool IsHovered() const { return m_isHovered; }

		protected:
			virtual void OnClick();

		private:
			bool ContainsPoint(const Vector2& point) const;

			std::string m_text;
			bool m_isPressed = false;
			bool m_isHovered = false;
			bool m_wasMousePressed = false;

			Color m_hoverColor;
			Color m_pressedColor;
			Color m_disabledColor;
			Color m_focusedBorderColor;

			ClickCallback m_onClick;
		};
	}
}
