#pragma once

#include "MenuItem.h"

#include <functional>
#include <memory>
#include <vector>

namespace KatanaEngine
{
	struct GameTime;
	class InputState;
	class SpriteBatch;

	class Menu
	{
	public:

		virtual ~Menu() = default;

		std::function<void(Menu&, int, int)> OnFocusChanged;

		void AddItem(const std::shared_ptr<MenuItem>& item);

		std::shared_ptr<MenuItem> GetItem(int index);
		std::shared_ptr<const MenuItem> GetItem(int index) const;
		
		int GetItemCount() const { return static_cast<int>(m_items.size()); }
		int GetSelectedIndex() const { return m_selectedIndex; }
		int GetVisibleItemCount() const { return m_visibleItemCount; }
		void SetVisibleItemCount(int count);
		bool GetWrapFocus() const { return m_wrapFocus; }
		void SetWrapFocus(bool wrapFocus) { m_wrapFocus = wrapFocus; }

		bool SetFocus(int index);
		bool FocusNext();
		bool FocusPrevious();
		void SelectFocused();
		void HandleInput(const InputState& inputState);

		virtual void Update(const GameTime& gameTime);
		virtual void Draw(SpriteBatch& spriteBatch) = 0;

	protected:
		Menu() = default;

		const std::vector<std::shared_ptr<MenuItem>>& GetItems() const { return m_items; }
		int GetFirstVisibleIndex() const { return m_firstVisibleIndex; }
		int GetLastVisibleIndex() const;

	private:
		void KeepFocusedItemVisible();
		int FindNextFocusableIndex(int startIndex, int step) const;
		int GetItemIndexAtPosition(const Vector2& position) const;

		std::vector<std::shared_ptr<MenuItem>> m_items;
		int m_selectedIndex = -1;
		int m_visibleItemCount = 0;
		int m_firstVisibleIndex = 0;
		bool m_wrapFocus = true;
	};
}
