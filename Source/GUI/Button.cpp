
#include "pch.h"

#include "Button.h"

#include "Core\Resource\Font.h"

namespace KatanaEngine
{
	namespace GUI
	{
		Button::Button(const std::string& text)
			: m_text(text)
			, m_hoverColor(Color(0.3f, 0.3f, 0.3f))
			, m_pressedColor(Color(0.1f, 0.1f, 0.1f))
			, m_disabledColor(Color(0.15f, 0.15f, 0.15f))
			, m_focusedBorderColor(Color::YELLOW)
		{
		}

		Button::Button()
			: Button("")
		{
		}

		void Button::HandleInput(const InputState& input)
		{
			if (!m_isEnabled || !m_isVisible) return;

			Vector2 mousePos = input.GetMousePosition();
			m_isHovered = ContainsPoint(mousePos);

			if (m_isHovered)
			{
				if (input.IsMouseButtonPressed(MouseButton::LEFT))
				{
					m_isPressed = true;
					m_wasMousePressed = true;
					SetFocused(true);
				}
			}

			if (m_wasMousePressed && input.IsMouseButtonReleased(MouseButton::LEFT))
			{
				if (m_isPressed && m_isHovered) OnClick();
				m_isPressed = false;
				m_wasMousePressed = false;
			}

			// Keyboard input (only when focused)
			if (m_isFocused)
			{
				if (input.IsKeyPressed(Keys::ENTER) || input.IsKeyPressed(Keys::SPACE)) m_isPressed = true;

				if (input.IsKeyReleased(Keys::ENTER) || input.IsKeyReleased(Keys::SPACE))
				{
					if (m_isPressed && !m_wasMousePressed)
					{
						m_isPressed = false;
						OnClick();
					}
				}
			}
		}

		void Button::Update(const GameTime& gameTime)
		{
			if (!m_isFocused && !m_wasMousePressed) m_isPressed = false;
		}

		void Button::Draw(SpriteBatch& spriteBatch)
		{
			if (!m_isVisible) return;

			// Determine colors based on state
			Color bgColor = m_backgroundColor;
			Color borderColor = m_borderColor;
			Color textColor = m_foregroundColor;

			if (!m_isEnabled)
			{
				bgColor = m_disabledColor;
				textColor = Color(0.5f, 0.5f, 0.5f);
			}
			else if (m_isPressed)
			{
				bgColor = m_pressedColor;
				textColor = Color(0.9f, 0.9f, 0.9f);
			}
			else if (m_isHovered)
			{
				bgColor = m_hoverColor;
				textColor = Color::WHITE;
			}

			if (m_isFocused || m_isHovered) borderColor = m_isHovered ? Color::WHITE : m_focusedBorderColor;

			// Draw button background
			Rectangle rect(m_position.X, m_position.Y, m_size.X, m_size.Y, 4.0f);
			spriteBatch.DrawPrimitive(rect, bgColor, borderColor, 2.0f);

			// Draw the button text (centered)
			if (m_font)
			{
				float textWidth = static_cast<float>(m_font->MeasureString(m_text));
				float lineHeight = static_cast<float>(m_font->GetLineHeight());

				Vector2 textPos(
					m_position.X + (m_size.X - textWidth) / 2.0f, m_position.Y + (m_size.Y - lineHeight) / 2.0f);
				spriteBatch.DrawString(m_font, m_text, textPos, textColor);
			}
		}

		void Button::OnClick()
		{
			if (!m_onClick) return;
			m_onClick(this);
		}

		bool Button::ContainsPoint(const Vector2& point) const
		{
			return point.X >= m_position.X && point.X < m_position.X + m_size.X && point.Y >= m_position.Y &&
			       point.Y < m_position.Y + m_size.Y;
		}
	}
}
