#include "pch.h"

#include "ScreenManager.h"
#include "Screen.h"
#include "../Game.h"

namespace KatanaEngine
{
	ScreenManager::ScreenManager(Game* pGame)
		: m_pGame(pGame)
		, m_pResourceManager(&pGame->GetResourceManager())
	{
	}

	void ScreenManager::AddScreen(std::shared_ptr<Screen> screen)
	{
		if (!screen) return;
		m_screensToAdd.push_back(screen);
	}

	void ScreenManager::RemoveScreen(std::shared_ptr<Screen> screen)
	{
		if (!screen) return;
		m_screensToRemove.push_back(screen);
	}

	void ScreenManager::RemoveScreen(Screen* screen)
	{
		for (auto& s : m_screens)
		{
			if (s.get() == screen)
			{
				m_screensToRemove.push_back(s);
				return;
			}
		}
	}

	void ScreenManager::Clear()
	{
		for (auto& screen : m_screens) m_screensToRemove.push_back(screen);
	}

	void ScreenManager::ClearImmediate()
	{
		m_screensToAdd.clear();
		m_screensToRemove.clear();

		for (auto& screen : m_screens)
		{
			screen->NotifyRemoved();
			screen->UnloadContent();
			screen->SetScreenManager({});
		}

		m_screens.clear();
	}

	void ScreenManager::HandleInput(const InputState& inputState)
	{
		ProcessPendingChanges();

		// Iterate from top (last) to bottom (first), stop at input-blocking screen
		for (auto it = m_screens.rbegin(); it != m_screens.rend(); ++it)
		{
			(*it)->HandleInput(inputState);
			if ((*it)->IsInputBlocking()) break;
		}
	}

	void ScreenManager::Update(const GameTime& gameTime)
	{
		// If there are no screens, check if we should exit the game
		if (m_screens.empty() && m_exitWithNoScreens) m_pGame->Exit();

		// Iterate from top (last) to bottom (first), stop at update-blocking screen
		for (auto it = m_screens.rbegin(); it != m_screens.rend(); ++it)
		{
			(*it)->Update(gameTime);
			if ((*it)->IsUpdateBlocking()) break;
		}
	}

	void ScreenManager::Draw(SpriteBatch& spriteBatch)
	{
		if (m_screens.empty()) return;

		// Find the lowest screen we need to draw (first draw-blocking from top)
		size_t startIndex = 0;
		for (size_t i = m_screens.size(); i > 0; --i)
		{
			if (m_screens[i - 1]->IsDrawBlocking())
			{
				startIndex = i - 1;
				break;
			}
		}

		// Draw from startIndex to top (bottom-to-top ordering)
		for (size_t i = startIndex; i < m_screens.size(); ++i) m_screens[i]->Draw(spriteBatch);
	}

	void ScreenManager::ProcessPendingChanges()
	{
		std::vector<std::shared_ptr<Screen>> screensToRemove;
		screensToRemove.swap(m_screensToRemove);

		// Process removals
		for (auto& screen : screensToRemove)
		{
			screen->NotifyRemoved();
			screen->UnloadContent();
			screen->SetScreenManager({});

			auto it = std::find(m_screens.begin(), m_screens.end(), screen);
			if (it != m_screens.end()) m_screens.erase(it);
		}

		std::vector<std::shared_ptr<Screen>> screensToAdd;
		screensToAdd.swap(m_screensToAdd);

		// Process additions
		for (auto& screen : screensToAdd)
		{
			screen->SetScreenManager(shared_from_this());
			screen->LoadContent(*m_pResourceManager);
			m_screens.push_back(screen);
			if (screen->ShouldShowOnAdd()) screen->Show();
		}
	}
}
