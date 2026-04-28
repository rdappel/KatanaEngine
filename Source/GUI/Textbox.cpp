
#include "pch.h"

#include "Textbox.h"

#include "Core/Resource/Font.h"

namespace KatanaEngine
{
	namespace GUI
	{
		Textbox::Textbox()
			: m_placeholderColor(Color(0.5f, 0.5f, 0.5f))
			, m_cursorColor(Color::WHITE)
			, m_focusedBorderColor(Color::YELLOW)
		{ }

		void Textbox::SetText(const std::string& text)
		{
			if (m_text != text)
			{
				m_text = text;
				m_cursorPosition = static_cast<int>(m_text.length());
				OnTextChanged();
			}
		}

		void Textbox::HandleInput(const InputState& input)
		{
			if (!m_isEnabled || !m_isVisible) return;

			Vector2 mousePosition = input.GetMousePosition();
			m_isHovered = ContainsPoint(mousePosition);

			// Mouse click to focus
			if (m_isHovered && input.IsMouseButtonPressed(MouseButton::LEFT))
			{
				SetFocused(true);
				m_cursorVisible = true;
				m_cursorBlinkTimer = 0.0f;
				// Position cursor at end for now (proper implementation would calculate from click position)
				// todo: calculate cursor position based on click
				m_cursorPosition = static_cast<int>(m_text.length());
			}

			if (!m_isFocused) return;

			bool isControlDown = input.IsKeyDown(Keys::LEFT_CONTROL) || input.IsKeyDown(Keys::RIGHT_CONTROL);

			if (input.IsKeyPressed(Keys::ENTER))
			{
				OnSubmit();
				return;
			}

			// Handle backspace (Ctrl+Backspace deletes whole word)
			if (input.IsKeyPressed(Keys::BACKSPACE))
			{
				if (m_cursorPosition > 0 && !m_text.empty())
				{
					if (isControlDown)
					{
						// Delete word backwards
						int newPosition = FindPreviousWordBoundary();
						m_text.erase(static_cast<size_t>(newPosition), static_cast<size_t>(m_cursorPosition - newPosition));
						m_cursorPosition = newPosition;
					}
					else
					{
						m_text.erase(static_cast<size_t>(m_cursorPosition - 1), 1);
						m_cursorPosition--;
					}
					OnTextChanged();
				}
				m_cursorVisible = true;
				m_cursorBlinkTimer = 0.0f;
				return;
			}

			// Handle delete (Ctrl+Delete deletes whole word forward)
			if (input.IsKeyPressed(Keys::DELETE))
			{
				if (m_cursorPosition < static_cast<int>(m_text.length()))
				{
					if (isControlDown)
					{
						// Delete word forwards
						int endPosition = FindNextWordBoundary();
						m_text.erase(static_cast<size_t>(m_cursorPosition), static_cast<size_t>(endPosition - m_cursorPosition));
					}
					else
					{
						m_text.erase(static_cast<size_t>(m_cursorPosition), 1);
					}
					OnTextChanged();
				}
				m_cursorVisible = true;
				m_cursorBlinkTimer = 0.0f;
				return;
			}

			// Handle cursor movement (Ctrl+Arrow moves by word)
			if (input.IsKeyPressed(Keys::LEFT))
			{
				if (isControlDown)
				{
					m_cursorPosition = FindPreviousWordBoundary();
				}
				else if (m_cursorPosition > 0)
				{
					m_cursorPosition--;
				}
				m_cursorVisible = true;
				m_cursorBlinkTimer = 0.0f;
				return;
			}

			if (input.IsKeyPressed(Keys::RIGHT))
			{
				if (isControlDown)
				{
					m_cursorPosition = FindNextWordBoundary();
				}
				else if (m_cursorPosition < static_cast<int>(m_text.length()))
				{
					m_cursorPosition++;
				}
				m_cursorVisible = true;
				m_cursorBlinkTimer = 0.0f;
				return;
			}

			if (input.IsKeyPressed(Keys::HOME))
			{
				m_cursorPosition = 0;
				m_cursorVisible = true;
				m_cursorBlinkTimer = 0.0f;
				return;
			}

			if (input.IsKeyPressed(Keys::END))
			{
				m_cursorPosition = static_cast<int>(m_text.length());
				m_cursorVisible = true;
				m_cursorBlinkTimer = 0.0f;
				return;
			}

			HandleCharacterInput(input);
		}

		void Textbox::HandleCharacterInput(const InputState& input)
		{
			if (m_maxTextLength > 0 && static_cast<int>(m_text.length()) >= m_maxTextLength) return;

			bool isShiftDown = input.IsKeyDown(Keys::LEFT_SHIFT) || input.IsKeyDown(Keys::RIGHT_SHIFT);

			// Check all possible character keys
			Keys keysToCheck[] = {
				Keys::A, Keys::B, Keys::C, Keys::D, Keys::E, Keys::F, Keys::G, Keys::H,
				Keys::I, Keys::J, Keys::K, Keys::L, Keys::M, Keys::N, Keys::O, Keys::P,
				Keys::Q, Keys::R, Keys::S, Keys::T, Keys::U, Keys::V, Keys::W, Keys::X,
				Keys::Y, Keys::Z,
				Keys::D0, Keys::D1, Keys::D2, Keys::D3, Keys::D4,
				Keys::D5, Keys::D6, Keys::D7, Keys::D8, Keys::D9,
				Keys::SPACE, Keys::COMMA, Keys::PERIOD, Keys::MINUS, Keys::EQUALS,
				Keys::SEMICOLON, Keys::QUOTE, Keys::SLASH, Keys::BACKSLASH,
				Keys::LEFT_BRACKET, Keys::RIGHT_BRACKET, Keys::TILDE
			};

			for (Keys key : keysToCheck)
			{
				if (input.IsKeyPressed(key))
				{
					char c = KeyToChar(key, isShiftDown);
					if (c != '\0')
					{
						std::string newText = m_text;
						newText.insert(m_cursorPosition, 1, c);

						if (m_maxPixelLength > 0 && m_font)
						{
							int textWidth = m_font->MeasureString(newText);
							if (textWidth > m_maxPixelLength) return;
						}

						m_text = newText;
						m_cursorPosition++;
						m_cursorVisible = true;
						m_cursorBlinkTimer = 0.0f;
						OnTextChanged();
						break;
					}
				}
			}
		}

		char Textbox::KeyToChar(Keys key, bool isShiftDown) const
		{
			if (key >= Keys::A && key <= Keys::Z)
			{
				int offset = static_cast<int>(key) - static_cast<int>(Keys::A);
				char c = static_cast<char>('a' + offset);
				return isShiftDown ? static_cast<char>(c - 32) : c;
			}

			if (key >= Keys::D0 && key <= Keys::D9)
			{
				int offset = static_cast<int>(key) - static_cast<int>(Keys::D0);
				if (isShiftDown)
				{
					const char* shiftSymbols = ")!@#$%^&*(";
					return shiftSymbols[offset];
				}
				return static_cast<char>('0' + offset);
			}

			switch (key)
			{
			case Keys::SPACE:        return ' ';
			case Keys::COMMA:        return isShiftDown ? '<' : ',';
			case Keys::PERIOD:       return isShiftDown ? '>' : '.';
			case Keys::MINUS:        return isShiftDown ? '_' : '-';
			case Keys::EQUALS:       return isShiftDown ? '+' : '=';
			case Keys::SEMICOLON:    return isShiftDown ? ':' : ';';
			case Keys::QUOTE:        return isShiftDown ? '"' : '\'';
			case Keys::SLASH:        return isShiftDown ? '?' : '/';
			case Keys::BACKSLASH:    return isShiftDown ? '|' : '\\';
			case Keys::LEFT_BRACKET: return isShiftDown ? '{' : '[';
			case Keys::RIGHT_BRACKET:return isShiftDown ? '}' : ']';
			case Keys::TILDE:        return isShiftDown ? '~' : '`';
			default: return '\0';
			}
		}

		void Textbox::Update(const GameTime& gameTime)
		{
			if (!m_isFocused) return;

			m_cursorBlinkTimer += static_cast<float>(gameTime.ElapsedTime);
			if (m_cursorBlinkTimer >= m_cursorBlinkRate)
			{
				m_cursorBlinkTimer = 0.0f;
				m_cursorVisible = !m_cursorVisible;
			}
		}

		void Textbox::Draw(SpriteBatch& spriteBatch)
		{
			if (!m_isVisible) return;

			Color borderColor = m_borderColor;
			if (!m_isEnabled)
			{
				borderColor = Color(0.3f, 0.3f, 0.3f);
			}
			else if (m_isFocused)
			{
				borderColor = m_focusedBorderColor;
			}
			else if (m_isHovered)
			{
				borderColor = Color(0.7f, 0.7f, 0.7f);
			}

			Rectangle rect(m_position.X, m_position.Y, m_size.X, m_size.Y, 2.0f);
			spriteBatch.DrawPrimitive(rect, m_backgroundColor, borderColor, m_isFocused ? 2.0f : 1.0f);

			if (!m_font) return;

			float padding = 8.0f;
			float lineHeight = static_cast<float>(m_font->GetLineHeight());
			float textY = m_position.Y + (m_size.Y - lineHeight) / 2.0f;

			const std::string& displayText = m_text.empty() ? m_placeholder : m_text;
			Color textColor = m_text.empty() ? m_placeholderColor : m_foregroundColor;

			if (!m_isEnabled) textColor = Color(0.5f, 0.5f, 0.5f);

			spriteBatch.DrawString(m_font, displayText, Vector2(m_position.X + padding, textY), textColor);

			if (m_isFocused && m_cursorVisible && m_isEnabled)
			{
				std::string textBeforeCursor = m_text.substr(0, m_cursorPosition);
				float textWidth = static_cast<float>(m_font->MeasureString(textBeforeCursor));
				float cursorX = m_position.X + padding + textWidth;
				float cursorTop = m_position.Y + 4.0f;
				float cursorBottom = m_position.Y + m_size.Y - 4.0f;

				Line cursorLine(cursorX, cursorTop, cursorX, cursorBottom);
				spriteBatch.DrawPrimitive(cursorLine, m_cursorColor, 2.0f);
			}
		}

		void Textbox::OnFocusChanged(bool focused)
		{
			Control::OnFocusChanged(focused);

			if (!focused) return;

			m_cursorVisible = true;
			m_cursorBlinkTimer = 0.0f;
		}

		void Textbox::OnTextChanged()
		{
			if (!m_onTextChanged) return;
			m_onTextChanged(this, m_text);
		}

		void Textbox::OnSubmit()
		{
			if (!m_onSubmit) return;
			m_onSubmit(this, m_text);
		}

		bool Textbox::ContainsPoint(const Vector2& point) const
		{
			return point.X >= m_position.X && point.X < m_position.X + m_size.X &&
				point.Y >= m_position.Y && point.Y < m_position.Y + m_size.Y;
		}

		bool Textbox::IsWordCharacter(char c) const
		{
			return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
				(c >= '0' && c <= '9') || c == '_';
		}

		int Textbox::FindPreviousWordBoundary() const
		{
			if (m_cursorPosition <= 0) return 0;

			int position = m_cursorPosition - 1;

			while (position > 0 && !IsWordCharacter(m_text[position])) position--;
			while (position > 0)
			{
				size_t prevIndex = static_cast<size_t>(position - 1);
				if (!IsWordCharacter(m_text[prevIndex])) break;
				position--;
			}

			return position;
		}

		int Textbox::FindNextWordBoundary() const
		{
			int length = static_cast<int>(m_text.length());
			if (m_cursorPosition >= length) return length;

			int position = m_cursorPosition;

			while (position < length && IsWordCharacter(m_text[position])) position++;
			while (position < length && !IsWordCharacter(m_text[position])) position++;

			return position;
		}
	}
}
