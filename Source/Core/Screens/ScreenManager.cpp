#include "pch.h"

#include "ScreenManager.h"
#include "Screen.h"
#include "../Game.h"

namespace KatanaEngine
{
	ScreenManager::ScreenManager(Game* pGame)
		: m_pGame(pGame)
		, m_pResourceManager(&pGame->GetResourceManager())
	{ }


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
		for (auto& screen : m_screens)
		{
			m_screensToRemove.push_back(screen);
		}
	}

	void ScreenManager::HandleInput(const InputState& inputState)
	{
		ProcessPendingChanges();

		// Iterate from top (last) to bottom (first), stop at input-blocking screen
		for (auto it = m_screens.rbegin(); it != m_screens.rend(); ++it)
		{
			(*it)->HandleInput(inputState);
			if ((*it)->IsInputBlocking())
			{
				break;
			}
		}
	}

	void ScreenManager::Update(const GameTime& gameTime)
	{
		// Iterate from top (last) to bottom (first), stop at update-blocking screen
		for (auto it = m_screens.rbegin(); it != m_screens.rend(); ++it)
		{
			(*it)->Update(gameTime);
			if ((*it)->IsUpdateBlocking())
			{
				break;
			}
		}
	}

	void ScreenManager::Draw(SpriteBatch& spriteBatch)
	{
		if (m_screens.empty())
		{
			return;
		}

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
		for (size_t i = startIndex; i < m_screens.size(); ++i)
		{
			m_screens[i]->Draw(spriteBatch);
		}
	}

	void ScreenManager::ProcessPendingChanges()
	{
		// Process removals
		for (auto& screen : m_screensToRemove)
		{
			screen->UnloadContent();
			screen->SetScreenManager(nullptr);

			auto it = std::find(m_screens.begin(), m_screens.end(), screen);
			if (it != m_screens.end())
			{
				m_screens.erase(it);
			}
		}
		m_screensToRemove.clear();

		// Process additions
		for (auto& screen : m_screensToAdd)
		{
			screen->SetScreenManager(this);
			screen->LoadContent(*m_pResourceManager);
			m_screens.push_back(screen);
		}
		m_screensToAdd.clear();
	}
}
