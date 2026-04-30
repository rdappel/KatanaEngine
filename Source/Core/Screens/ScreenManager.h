#pragma once

#include <memory>
#include <vector>

#include "Screen.h"
#include "..\GameTime.h"
#include "..\Resource\ResourceManager.h"
#include "..\Graphics\SpriteBatch.h"

namespace KatanaEngine
{
	class Game;

	class ScreenManager
	{
	public:

		ScreenManager(Game* pGame);

		~ScreenManager() = default;

		ScreenManager(const ScreenManager&) = delete;
		ScreenManager& operator=(const ScreenManager&) = delete;

		Game& GetGame() const { return *m_pGame; }

		void AddScreen(std::shared_ptr<Screen> screen);
		void RemoveScreen(std::shared_ptr<Screen> screen);
		void RemoveScreen(Screen* screen);
		void Clear();

		void HandleInput(const InputState& inputState);
		void Update(const GameTime& gameTime);
		void Draw(SpriteBatch& spriteBatch);

		size_t GetScreenCount() const { return m_screens.size(); }

	private:
		void ProcessPendingChanges();

		Game* m_pGame = nullptr;
		ResourceManager* m_pResourceManager = nullptr;

		std::vector<std::shared_ptr<Screen>> m_screens;
		std::vector<std::shared_ptr<Screen>> m_screensToAdd;
		std::vector<std::shared_ptr<Screen>> m_screensToRemove;
	};
}
