#include "pch.h"

#include "MenuItem.h"

namespace KatanaEngine
{
	void MenuItem::SetFocused(bool focused)
	{
		if (focused && !m_isEnabled)
		{
			focused = false;
		}

		if (m_isFocused == focused)
		{
			return;
		}

		m_isFocused = focused;
		if (m_isFocused && OnFocus)
		{
			OnFocus(*this);
		}
	}

	void MenuItem::SetEnabled(bool enabled)
	{
		m_isEnabled = enabled;
		if (!m_isEnabled)
		{
			SetFocused(false);
		}
	}

	void MenuItem::Select()
	{
		if (!m_isEnabled)
		{
			return;
		}

		if (OnSelect)
		{
			OnSelect(*this);
		}
	}
}
