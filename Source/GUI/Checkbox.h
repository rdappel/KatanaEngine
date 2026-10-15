#pragma once

#include "Control.h"

namespace KatanaEngine
{
	namespace GUI
	{
		class Checkbox : public Control
		{
			using ChangedCallback = std::function<void(Checkbox*)>;

		public:
			Checkbox();
			Checkbox(const std::string& label);
			virtual ~Checkbox() = default;

			void HandleInput(const InputState& input) override;
			void Update(const GameTime& gameTime) override {};
			void Draw(SpriteBatch& spriteBatch) override;

			bool IsChecked() const { return m_isChecked; }
			void SetChecked(bool checked);

			const std::string& GetLabel() const { return m_label; }
			void SetLabel(const std::string& label) { m_label = label; }

			void SetBoxSize(float size) { m_boxSize = size; }
			void SetCheckColor(const Color& color) { m_checkColor = color; }
			void SetBoxColor(const Color& color) { m_boxColor = color; }
			void SetHoverColor(const Color& color) { m_hoverColor = color; }
			void SetLabelColor(const Color& color) { m_labelColor = color; }
			void SetLabelOffsetY(float offset) { m_labelOffsetY = offset; }

			void SetOnChanged(ChangedCallback callback) { m_onChanged = callback; }

			bool IsHovered() const { return m_isHovered; }

		protected:
			virtual void OnChanged();

		private:
			bool ContainsPoint(const Vector2& point) const;
			Region GetBoxBounds() const;

			std::string m_label;
			bool m_isChecked = false;
			bool m_isHovered = false;
			bool m_wasMousePressed = false;

			float m_boxSize = 16.0f;
			float m_labelSpacing = 8.0f;
			float m_labelOffsetY = 0.0f;

			Color m_boxColor = Color::WHITE;
			Color m_checkColor = Color::GREEN;
			Color m_hoverColor = Color::YELLOW;
			Color m_labelColor = Color::WHITE;
			Color m_disabledColor = Color((uint8_t)100, (uint8_t)100, (uint8_t)100);
			Color m_focusedBorderColor = Color::YELLOW;

			ChangedCallback m_onChanged;
		};
	}
}
