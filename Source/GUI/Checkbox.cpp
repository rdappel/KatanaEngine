
#include "pch.h"

#include "Checkbox.h"

namespace KatanaEngine
{
	namespace GUI
	{
		Checkbox::Checkbox()
			: m_label("")
		{
			SetSize(100, 20);
		}

		Checkbox::Checkbox(const std::string& label)
			: m_label(label)
		{
			SetSize(100, 20);
		}

		void Checkbox::HandleInput(const InputState& input)
		{
			if (!IsEnabled() || !IsVisible()) return;

			Vector2 mousePos = input.GetMousePosition();
			m_isHovered = ContainsPoint(mousePos);

			bool isMousePressed = input.IsMouseButtonDown(MouseButton::LEFT);

			if (m_isHovered && input.IsMouseButtonReleased(MouseButton::LEFT) && m_wasMousePressed)
				SetChecked(!m_isChecked);

			m_wasMousePressed = m_isHovered && isMousePressed;

			if (IsFocused())
			{
				if (input.IsKeyReleased(Keys::ENTER) || input.IsKeyReleased(Keys::SPACE)) SetChecked(!m_isChecked);
			}
		}

		void Checkbox::Draw(SpriteBatch& spriteBatch)
		{
			if (!IsVisible()) return;

			Region boxBounds = GetBoxBounds();

			Color checkColor = IsEnabled() ? m_checkColor : m_disabledColor;
			Color labelColor = IsEnabled() ? m_labelColor : m_disabledColor;

			Color borderColor = !IsEnabled()
			                        ? m_disabledColor
			                        : (m_isHovered ? m_hoverColor : (IsFocused() ? m_focusedBorderColor : m_boxColor));

			Rectangle boxRect(boxBounds.X, boxBounds.Y, boxBounds.Width, boxBounds.Height, 2.0f);
			spriteBatch.DrawPrimitive(boxRect, Color::TRANSPARENT, borderColor, 2.0f);

			if (m_isChecked)
			{
				// Draw an X inside the box
				// todo: allow a custom checkmark texture
				float padding = 4.0f;
				Vector2 topLeft(boxBounds.X + padding, boxBounds.Y + padding);
				Vector2 topRight(boxBounds.X + boxBounds.Width - padding, boxBounds.Y + padding);
				Vector2 bottomLeft(boxBounds.X + padding, boxBounds.Y + boxBounds.Height - padding);
				Vector2 bottomRight(boxBounds.X + boxBounds.Width - padding, boxBounds.Y + boxBounds.Height - padding);

				Line line1(topLeft, bottomRight);
				spriteBatch.DrawPrimitive(line1, checkColor, 2.0f);

				Line line2(topRight, bottomLeft);
				spriteBatch.DrawPrimitive(line2, checkColor, 2.0f);
			}

			if (!m_label.empty() && GetFont())
			{
				float lineHeight = 14.0f; // Works well for most fonts at typical sizes
				Vector2 labelPos(
					boxBounds.X + boxBounds.Width + m_labelSpacing,
					boxBounds.Y + (boxBounds.Height - lineHeight) / 2.0f + m_labelOffsetY - 3.0f);
				spriteBatch.DrawString(GetFont(), m_label, labelPos, labelColor);
			}
		}

		void Checkbox::SetChecked(bool checked)
		{
			if (m_isChecked != checked)
			{
				m_isChecked = checked;
				OnChanged();
			}
		}

		void Checkbox::OnChanged()
		{
			if (m_onChanged) m_onChanged(this);
		}

		bool Checkbox::ContainsPoint(const Vector2& point) const
		{
			Region bounds = GetBounds();
			return point.X >= bounds.X && point.X <= bounds.X + bounds.Width && point.Y >= bounds.Y &&
			       point.Y <= bounds.Y + bounds.Height;
		}

		Region Checkbox::GetBoxBounds() const
		{
			Vector2 pos = GetPosition();
			return Region(pos.X, pos.Y, m_boxSize, m_boxSize);
		}
	}
}
