#pragma once

#include <memory>
#include <vector>

namespace KatanaEngine
{
	class Game;
	struct GameTime;
	class InputState;
	class ResourceManager;
	class Screen;
	class SpriteBatch;

	class ScreenManager
	{
	public:
		ScreenManager(Game& game, ResourceManager& resourceManager)
			: m_game(game)
			, m_resourceManager(resourceManager)
		{ }

		~ScreenManager() = default;

		ScreenManager(const ScreenManager&) = delete;
		ScreenManager& operator=(const ScreenManager&) = delete;

		Game& GetGame() const { return m_game; }

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

		Game& m_game;
		ResourceManager& m_resourceManager;
		std::vector<std::shared_ptr<Screen>> m_screens;
		std::vector<std::shared_ptr<Screen>> m_screensToAdd;
		std::vector<std::shared_ptr<Screen>> m_screensToRemove;
	};
}
