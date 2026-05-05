#pragma once

#include <memory>
#include <string>

#include "Input/InputState.h"
#include "Input/SystemMouseCursor.h"
#include "Graphics/SpriteBatch.h"
#include "Resource/ResourceManager.h"
#include "Screens/ScreenManager.h"
#include "GameTime.h"

namespace KatanaEngine
{
	// Forward declarations
	struct PlatformContext;
	struct Color;

	struct GameConfig
	{
		std::string ContentRoot = "../../../Content/";
		int ScreenWidth = 1600;
		int ScreenHeight = 900;
		bool Fullscreen = false;
	};

	class Game
	{
	public:
		Game(int argc, char** argv, const GameConfig& config);

		Game() : Game(0, nullptr) { }
		Game(const GameConfig& config) : Game(0, nullptr, config) { }
		Game(int argc, char** argv) : Game(argc, argv, GameConfig()) { }

		Game(const Game&) = delete;
		Game& operator=(const Game&) = delete;
		Game(Game&&) = delete;
		Game& operator=(Game&&) = delete;

		virtual ~Game();

		int Run();
		void Exit() { m_isRunning = false; }

		int GetScreenWidth() const { return m_screenWidth; }
		int GetScreenHeight() const { return m_screenHeight; }
		Vector2 GetScreenSize() const { return Vector2(m_screenWidth, m_screenHeight); }
		void SetScreenSize(int width, int height);

		const std::string& GetTitle() const { return m_title; }
		void SetTitle(const std::string& title);

		double GetTargetFrameRate() const { return m_targetFrameRate; }
		void SetTargetFrameRate(double fps);
		bool SetSystemMouseCursor(SystemMouseCursor cursor);

		ScreenManager& GetScreenManager() { return m_screenManager; }
		const ScreenManager& GetScreenManager() const { return m_screenManager; }

		ResourceManager& GetResourceManager() { return m_resourceManager; }
		const ResourceManager& GetResourceManager() const { return m_resourceManager; }
		
	protected:
		void Clear(const Color& color);

		virtual void LoadContent(ResourceManager& resourceManager) {}
		virtual void UnloadContent() {}

		virtual void HandleInput(const InputState& input) = 0;
		virtual void Update(const GameTime& gameTime) = 0;
		virtual void Draw(SpriteBatch& spriteBatch) = 0;

	private:
		bool InitializeAllegro();
		int Shutdown();
		void GameLoop();
		void UpdateScreenSizeFromDisplay();

	private:
		std::unique_ptr<PlatformContext> m_pContext;

		// Core services
		InputState m_inputState;
		SpriteBatch m_spriteBatch;
		ResourceManager m_resourceManager;
		ScreenManager m_screenManager;

		GameConfig m_config;

		int m_screenWidth = 0;
		int m_screenHeight = 0;
		std::string m_title = "KatanaEngine Game";

		double m_targetFrameRate = 60.0;
		double m_targetElapsedTime = 1.0 / 60.0;
		double m_totalTime = 0.0;
		double m_accumulatedTime = 0.0;
		double m_previousTime = 0.0;

		bool m_isRunning = false;
		bool m_isInitialized = false;

		static const int MAX_FRAME_SKIP = 5;
	};
}
