#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "Input/InputState.h"
#include "Input/MouseCursor.h"
#include "Graphics/SpriteBatch.h"
#include "Resource/ResourceManager.h"
#include "Screens/ScreenManager.h"
#include "GameTime.h"

namespace KatanaEngine
{
	// Forward declarations
	struct PlatformContext;
	struct Color;

	enum class GraphicsBackend
	{
		Unknown,
		OpenGL,
		Direct3D
	};

	struct GameConfig
	{
		std::string ContentRoot = "../../../Content/";
		int ScreenWidth = 1600;
		int ScreenHeight = 900;
		bool Fullscreen = false;
		bool PreferDirect3D = false;  // On Windows, prefer D3D over OpenGL

		// Windows only: when set (an HWND), the game window is embedded as a borderless child
		// filling this window's client area instead of opening its own top-level window.
		// See Core/Hosting/GameHost.h for running a hosted game from another application.
		void* ParentWindow = nullptr;
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
		bool SetMouseCursor(MouseCursor cursor);

       ScreenManager& GetScreenManager() { return *m_screenManager; }
		const ScreenManager& GetScreenManager() const { return *m_screenManager; }

		ResourceManager& GetResourceManager() { return m_resourceManager; }
		const ResourceManager& GetResourceManager() const { return m_resourceManager; }

		GraphicsBackend GetGraphicsBackend() const { return m_graphicsBackend; }

		// --- Hosting (see Core/Hosting/GameHost.h) ---
		// These are safe to call from any thread; requests are applied on the game thread
		// at the start of the next loop iteration.

		using HostMessageCallback = std::function<void(const std::string&)>;

		void RequestExit();
		void RequestResize(int width, int height);
		void PostHostMessage(const std::string& message);

		// Set before Run(). Called on the game thread.
		void SetHostMessageCallback(HostMessageCallback callback) { m_hostMessageCallback = std::move(callback); }
		void SetStartedCallback(std::function<void()> callback) { m_startedCallback = std::move(callback); }

	protected:
		void Clear(const Color& color);

		virtual void LoadContent(ResourceManager& resourceManager) {}
		virtual void UnloadContent() {}

		virtual void HandleInput(const InputState& input) = 0;
		virtual void Update(const GameTime& gameTime) = 0;
		virtual void Draw(SpriteBatch& spriteBatch) = 0;

		// Called on the game thread for each message posted with PostHostMessage().
		virtual void OnHostMessage(const std::string& message) {}

		// Sends a message to the hosting application (no-op when not hosted).
		void SendHostMessage(const std::string& message) const;

	private:
		bool InitializeAllegro();
		int Shutdown();
		void GameLoop();
		void UpdateScreenSizeFromDisplay();
		void ProcessHostRequests();

	private:
		std::unique_ptr<PlatformContext> m_pContext;

		// Core services
		InputState m_inputState;
		SpriteBatch m_spriteBatch;
		ResourceManager m_resourceManager;
      std::shared_ptr<ScreenManager> m_screenManager;

		GameConfig m_config;

		int m_screenWidth = 0;
		int m_screenHeight = 0;
		std::string m_title = "KatanaEngine Game";

		double m_targetFrameRate = 60.0;
		double m_targetElapsedTime = 1.0 / 60.0;
		double m_totalTime = 0.0;
		double m_accumulatedTime = 0.0;
		double m_previousTime = 0.0;

		GraphicsBackend m_graphicsBackend = GraphicsBackend::Unknown;

		bool m_isRunning = false;
		bool m_isInitialized = false;

		// Hosting: written by other threads, read on the game thread.
		std::atomic<bool> m_hasHostRequests{ false };
		std::mutex m_hostMutex;
		bool m_exitRequested = false;
		int m_requestedWidth = 0;
		int m_requestedHeight = 0;
		std::vector<std::string> m_pendingHostMessages;
		std::vector<std::string> m_processingHostMessages;

		HostMessageCallback m_hostMessageCallback;
		std::function<void()> m_startedCallback;

		static const int MAX_FRAME_SKIP = 5;
	};
}
