#pragma once

#include "Control.h"

namespace KatanaEngine
{
	namespace GUI
	{
		class Panel : public Control
		{
		public:
			Panel();
			Panel(const std::string& title);
			virtual ~Panel() = default;

			void HandleInput(const InputState& input) override;
			void Update(const GameTime& gameTime) override;
			void Draw(SpriteBatch& spriteBatch) override;

			const std::string& GetTitle() const { return m_title; }
			void SetTitle(const std::string& title) { m_title = title; }

			void SetTitleBarHeight(float height) { m_titleBarHeight = height; }
			void SetTitleBarColor(const Color& color) { m_titleBarColor = color; }
			void SetPadding(float padding) { m_padding = padding; }
			void SetShowBorder(bool show) { m_showBorder = show; }
			void SetDraggable(bool draggable) { m_isDraggable = draggable; }

			bool IsHovered() const { return m_isHovered; }
			bool IsDraggable() const { return m_isDraggable; }
			bool IsDragging() const { return m_isDragging; }
			bool JustStartedDragging() const { return m_justStartedDragging; }
			void ClearDragStartFlag() { m_justStartedDragging = false; }

			void AddControl(Control* control);
			void RemoveControl(Control* control);
			void ClearControls();
			const std::vector<Control*>& GetControls() const { return m_controls; }

			Region GetContentBounds() const;

		private:
			bool IsTitleBarHit(const Vector2& point) const;
			Region GetTitleBarBounds() const;
			bool ContainsPoint(const Vector2& point) const;

			std::string m_title;
			std::vector<Control*> m_controls;

			float m_titleBarHeight = 30.0f;
			float m_padding = 10.0f;
			bool m_showBorder = true;
			bool m_isDraggable = false;
			bool m_isDragging = false;
			bool m_justStartedDragging = false;
			bool m_isHovered = false;
			Vector2 m_dragOffset;

			Color m_titleBarColor = Color(0.15f, 0.15f, 0.15f);
		};
	}
}
