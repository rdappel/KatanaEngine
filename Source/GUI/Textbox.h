#pragma once

#include "Control.h"

namespace KatanaEngine
{
	namespace GUI
	{
		class Textbox : public Control
		{
			using TextChangedCallback = std::function<void(Textbox*, const std::string&)>;
			using SubmitCallback = std::function<void(Textbox*, const std::string&)>;

		public:
			Textbox();
			virtual ~Textbox() = default;

			void HandleInput(const InputState& input) override;
			void Update(const GameTime& gameTime) override;
			void Draw(SpriteBatch& spriteBatch) override;

			const std::string& GetText() const { return m_text; }
			void SetText(const std::string& text);

			// Max length (0 = unlimited)
			int GetMaxTextLength() const { return m_maxTextLength; }
			void SetMaxTextLength(int maxLength) { m_maxTextLength = maxLength; }

			int GetMaxPixelLength() const { return m_maxPixelLength; }
			void SetMaxPixelLength(int maxPixels) { m_maxPixelLength = maxPixels; }

			const std::string& GetPlaceholder() const { return m_placeholder; }
			void SetPlaceholder(const std::string& placeholder) { m_placeholder = placeholder; }
			void SetPlaceholderColor(const Color& color) { m_placeholderColor = color; }

			void SetCursorColor(const Color& color) { m_cursorColor = color; }
			float GetCursorBlinkRate() const { return m_cursorBlinkRate; }
			void SetCursorBlinkRate(float rate) { m_cursorBlinkRate = rate; }

			void SetFocusedBorderColor(const Color& color) { m_focusedBorderColor = color; }

			void SetOnTextChanged(TextChangedCallback callback) { m_onTextChanged = callback; }
			void SetOnSubmit(SubmitCallback callback) { m_onSubmit = callback; }

		protected:
			virtual void OnTextChanged();
			virtual void OnSubmit();

			void OnFocusChanged(bool focused) override;

		private:
			void HandleCharacterInput(const InputState& input);
			char KeyToChar(Keys key, bool shift) const;
			bool ContainsPoint(const Vector2& point) const;

			int FindPreviousWordBoundary() const;
			int FindNextWordBoundary() const;
			bool IsWordCharacter(char c) const;

			std::string m_text;
			std::string m_placeholder;
			int m_maxTextLength = 0;
			int m_maxPixelLength = 0;
			int m_cursorPosition = 0;

			float m_cursorBlinkTimer = 0.0f;
			float m_cursorBlinkRate = 0.5f;
			bool m_cursorVisible = true;
			bool m_isHovered = false;

			Color m_placeholderColor;
			Color m_cursorColor;
			Color m_focusedBorderColor;

			TextChangedCallback m_onTextChanged;
			SubmitCallback m_onSubmit;
		};
	}
}
