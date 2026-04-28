
#include "pch.h"

#include "Control.h"

namespace KatanaEngine
{
	namespace GUI
	{
		Control::Control()
			: m_position(0, 0)
			, m_size(100, 30)
			, m_backgroundColor(Color(0.2f, 0.2f, 0.2f))
			, m_foregroundColor(Color::WHITE)
			, m_borderColor(Color(0.5f, 0.5f, 0.5f))
		{ }

		Region Control::GetBounds() const
		{
			return Region(
				static_cast<int>(m_position.X),
				static_cast<int>(m_position.Y),
				static_cast<int>(m_size.X),
				static_cast<int>(m_size.Y));
		}

		void Control::SetFocused(bool focused)
		{
			if (m_isFocused != focused)
			{
				m_isFocused = focused;
				OnFocusChanged(focused);
			}
		}

		void Control::OnFocusChanged(bool focused)
		{
			if (!m_onFocusChanged) return;
			m_onFocusChanged(this, focused);
		}
	}
}
