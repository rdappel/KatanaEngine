
#include "pch.h"

#include "Panel.h"

namespace KatanaEngine
{
	namespace GUI
	{
		Panel::Panel()
			: m_title("")
		{
			SetSize(200, 150);
			SetBackgroundColor(Color(0.1f, 0.1f, 0.1f, 1.0f));
		}

		Panel::Panel(const std::string& title)
			: m_title(title)
		{
			SetSize(200, 150);
			SetBackgroundColor(Color(0.1f, 0.1f, 0.1f, 1.0f));
		}

		void Panel::HandleInput(const InputState& input)
		{
			if (!IsEnabled() || !IsVisible()) return;

			Vector2 mousePos = input.GetMousePosition();
			m_isHovered = ContainsPoint(mousePos);

			// Handle dragging
			if (m_isDraggable && !m_title.empty())
			{
				bool isMouseDown = input.IsMouseButtonDown(MouseButton::LEFT);
				bool isMousePressed = input.IsMouseButtonPressed(MouseButton::LEFT);

				if (isMousePressed && IsTitleBarHit(mousePos))
				{
					// Start dragging
					m_isDragging = true;
					m_justStartedDragging = true;
					m_dragOffset = mousePos - GetPosition();
				}
				else if (!isMouseDown && m_isDragging)
				{
					// Stop dragging
					m_isDragging = false;
				}

				if (m_isDragging && isMouseDown)
				{
					Vector2 oldPosition = GetPosition();
					Vector2 newPosition = mousePos - m_dragOffset;
					Vector2 offset = newPosition - oldPosition;
					SetPosition(newPosition);

					for (auto* control : m_controls)
					{
						if (!control) continue;

						Vector2 childPos = control->GetPosition();
						control->SetPosition(childPos + offset);
					}

					return;
				}
			}

			// Pass input to all child controls
			for (auto* control : m_controls)
				if (control) control->HandleInput(input);
		}

		void Panel::Update(const GameTime& gameTime)
		{
			for (auto* control : m_controls)
				if (control) control->Update(gameTime);
		}

		void Panel::Draw(SpriteBatch& spriteBatch)
		{
			if (!IsVisible()) return;

			Vector2 pos = GetPosition();
			Vector2 size = GetSize();

			Rectangle panelRect(pos.X, pos.Y, size.X, size.Y, 4.0f);
			Color bgColor = GetBackgroundColor();
			Color borderColor = m_showBorder ? GetBorderColor() : Color::TRANSPARENT;
			spriteBatch.DrawPrimitive(panelRect, bgColor, borderColor, 2.0f);

			if (!m_title.empty() && GetFont())
			{
				// Draw title bar background
				Color titleBgColor = m_isDragging ? Color(0.2f, 0.2f, 0.25f) : m_titleBarColor;
				Rectangle titleBarRect(pos.X, pos.Y, size.X, m_titleBarHeight, 4.0f);
				spriteBatch.DrawPrimitive(titleBarRect, titleBgColor, Color::TRANSPARENT, 0);

				// Draw title text
				Vector2 titlePos(pos.X + m_padding, pos.Y + (m_titleBarHeight - 14.0f) / 2.0f);
				spriteBatch.DrawString(GetFont(), m_title, titlePos, GetForegroundColor());
			}

			for (auto* control : m_controls)
			{
				if (!control) continue;
				control->Draw(spriteBatch);
			}
		}

		void Panel::AddControl(Control* control)
		{
			if (!control) return;
			m_controls.push_back(control);
		}

		void Panel::RemoveControl(Control* control)
		{
			if (!control) return;

			auto it = std::find(m_controls.begin(), m_controls.end(), control);
			if (it != m_controls.end()) m_controls.erase(it);
		}

		void Panel::ClearControls()
		{
			m_controls.clear();
		}

		Region Panel::GetContentBounds() const
		{
			Vector2 position = GetPosition();
			Vector2 size = GetSize();

			float contentY = position.Y + (m_title.empty() ? m_padding : m_titleBarHeight + m_padding);
			float contentHeight = size.Y - (m_title.empty() ? m_padding * 2 : m_titleBarHeight + m_padding * 2);

			return Region(position.X + m_padding, contentY, size.X - m_padding * 2, contentHeight);
		}

		bool Panel::IsTitleBarHit(const Vector2& point) const
		{
			if (m_title.empty()) return false;

			Region titleBar = GetTitleBarBounds();
			return point.X >= titleBar.X && point.X <= titleBar.X + titleBar.Width && point.Y >= titleBar.Y &&
			       point.Y <= titleBar.Y + titleBar.Height;
		}

		Region Panel::GetTitleBarBounds() const
		{
			Vector2 position = GetPosition();
			Vector2 size = GetSize();
			return Region(position.X, position.Y, size.X, m_titleBarHeight);
		}

		bool Panel::ContainsPoint(const Vector2& point) const
		{
			Vector2 position = GetPosition();
			Vector2 size = GetSize();
			return point.X >= position.X && point.X <= position.X + size.X && point.Y >= position.Y &&
			       point.Y <= position.Y + size.Y;
		}
	}
}
