#include "pch.h"

#include "Menu.h"
#include "Core/Input/InputState.h"

namespace KatanaEngine
{
	void Menu::AddItem(const std::shared_ptr<MenuItem>& item)
	{
		if (!item) return;

		m_items.push_back(item);
		if (m_visibleItemCount <= 0)
		{
			m_visibleItemCount = static_cast<int>(m_items.size());
		}

		if (m_selectedIndex < 0)
		{
			SetFocus(0);
		}
	}

	std::shared_ptr<MenuItem> Menu::GetItem(int index)
	{
		if (index < 0 || index >= static_cast<int>(m_items.size()))
		{
			return nullptr;
		}

		return m_items[index];
	}

	std::shared_ptr<const MenuItem> Menu::GetItem(int index) const
	{
		if (index < 0 || index >= static_cast<int>(m_items.size()))
		{
			return nullptr;
		}

		return m_items[index];
	}

	void Menu::SetVisibleItemCount(int count)
	{
		m_visibleItemCount = count <= 0 ? static_cast<int>(m_items.size()) : count;
		KeepFocusedItemVisible();
	}

	bool Menu::SetFocus(int index)
	{
		if (index < 0 || index >= static_cast<int>(m_items.size()) || index == m_selectedIndex || !m_items[index]->IsEnabled())
		{
			return false;
		}

		const int previousIndex = m_selectedIndex;
		if (previousIndex >= 0 && previousIndex < static_cast<int>(m_items.size()))
		{
			m_items[previousIndex]->SetFocused(false);
		}

		m_selectedIndex = index;
		m_items[m_selectedIndex]->SetFocused(true);
		KeepFocusedItemVisible();

		if (OnFocusChanged)
		{
			OnFocusChanged(*this, previousIndex, m_selectedIndex);
		}

		return true;
	}

	bool Menu::FocusNext()
	{
		if (m_items.empty())
		{
			return false;
		}

		int nextIndex = FindNextFocusableIndex(m_selectedIndex, 1);
		return SetFocus(nextIndex);
	}

	bool Menu::FocusPrevious()
	{
		if (m_items.empty())
		{
			return false;
		}

		int previousIndex = FindNextFocusableIndex(m_selectedIndex, -1);
		return SetFocus(previousIndex);
	}

	void Menu::SelectFocused()
	{
		if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_items.size()))
		{
			return;
		}

		m_items[m_selectedIndex]->Select();
	}

	void Menu::HandleInput(const InputState& inputState)
	{
		if (inputState.IsKeyPressed(Keys::DOWN) || inputState.IsKeyPressed(Keys::S))
		{
			FocusNext();
		}
		else if (inputState.IsKeyPressed(Keys::UP) || inputState.IsKeyPressed(Keys::W))
		{
			FocusPrevious();
		}

		if (inputState.IsKeyPressed(Keys::ENTER) || inputState.IsKeyPressed(Keys::SPACE))
		{
			SelectFocused();
		}

		const int hoveredIndex = GetItemIndexAtPosition(inputState.GetMousePosition());
		if (hoveredIndex >= 0)
		{
			SetFocus(hoveredIndex);
			if (inputState.IsMouseButtonPressed(MouseButton::LEFT))
			{
				SelectFocused();
			}
		}
	}

	void Menu::Update(const GameTime& gameTime)
	{
		const int lastVisibleIndex = GetLastVisibleIndex();
		for (int i = m_firstVisibleIndex; i <= lastVisibleIndex; ++i)
		{
			if (m_items[i])
			{
				m_items[i]->Update(gameTime);
			}
		}
	}

	int Menu::GetLastVisibleIndex() const
	{
		if (m_items.empty())
		{
			return -1;
		}

		const int visibleCount = m_visibleItemCount <= 0 ? static_cast<int>(m_items.size()) : m_visibleItemCount;
		const int lastIndex = m_firstVisibleIndex + visibleCount - 1;
		return Math::Min(lastIndex, static_cast<int>(m_items.size()) - 1);
	}

	void Menu::KeepFocusedItemVisible()
	{
		if (m_selectedIndex < 0 || m_items.empty()) return;

		const int visibleCount = m_visibleItemCount <= 0 ? static_cast<int>(m_items.size()) : m_visibleItemCount;
		if (m_selectedIndex < m_firstVisibleIndex)
		{
			m_firstVisibleIndex = m_selectedIndex;
		}
		else if (m_selectedIndex >= m_firstVisibleIndex + visibleCount)
		{
			m_firstVisibleIndex = m_selectedIndex - visibleCount + 1;
		}

		const int maxFirstVisible = Math::Max(0, static_cast<int>(m_items.size()) - visibleCount);
		m_firstVisibleIndex = Math::Clamp(m_firstVisibleIndex, 0, maxFirstVisible);
	}

	int Menu::GetItemIndexAtPosition(const Vector2& position) const
	{
		const int lastVisibleIndex = GetLastVisibleIndex();
		for (int i = m_firstVisibleIndex; i <= lastVisibleIndex; ++i)
		{
			if (m_items[i] && m_items[i]->IsEnabled() && m_items[i]->GetBounds().Contains(position)) return i;
		}

		return -1;
	}

	int Menu::FindNextFocusableIndex(int startIndex, int step) const
	{
		if (m_items.empty())
		{
			return -1;
		}

		const int itemCount = static_cast<int>(m_items.size());
		int index = startIndex;

		for (int attempts = 0; attempts < itemCount; ++attempts)
		{
			index += step;

			if (index < 0)
			{
				if (!m_wrapFocus) return startIndex;
				index = itemCount - 1;
			}
			else if (index >= itemCount)
			{
				if (!m_wrapFocus) return startIndex;
				index = 0;
			}

			if (m_items[index] && m_items[index]->IsEnabled())
			{
				return index;
			}
		}

		return startIndex;
	}
}
