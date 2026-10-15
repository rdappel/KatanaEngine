#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace KatanaEngine
{
	class Game;
	struct GameConfig;

	// Runs a Game embedded in another application's window (e.g. a WinForms control),
	// on its own thread. The game's window becomes a child of the host window and
	// the two sides talk through string messages.
	//
	// Threading:
	// - Start, Resize, Post and Stop are called from the host's UI thread.
	// - The message callback is invoked on the game thread; the host must marshal
	//   to its UI thread itself.
	// - Start and Stop wait for the game thread while still servicing window messages
	//   sent to the calling thread, so the parent/child windows cannot deadlock.
	//
	// Most games don't use this class directly: they expose it to .NET through
	// KATANA_DEFINE_HOST_EXPORTS (see HostExports.h).
	class GameHost
	{
	public:
		// Receives a config with ParentWindow already set; returns the game to run.
		using GameFactory = std::function<std::unique_ptr<Game>(const GameConfig& config)>;
		using MessageCallback = std::function<void(const std::string& message)>;

		GameHost() = default;
		~GameHost();

		GameHost(const GameHost&) = delete;
		GameHost& operator=(const GameHost&) = delete;

		// Blocks until the game has loaded its content (true) or failed to start (false).
		bool Start(void* parentWindow, GameFactory factory, MessageCallback onMessage);

		void Resize(int width, int height);
		void Post(const std::string& message);

		// Blocks until the game thread has shut down.
		void Stop();

		bool IsRunning() const { return m_state.load() == State::Running; }

	private:
		enum class State
		{
			Stopped,
			Starting,
			Running,
			Failed
		};

		void RunGameThread(void* parentWindow, GameFactory factory, MessageCallback onMessage);

		std::thread m_thread;
		std::atomic<State> m_state{State::Stopped};
		std::atomic<bool> m_threadFinished{true};

		std::mutex m_gameMutex;
		Game* m_pGame = nullptr; // owned by the game thread; guarded by m_gameMutex
	};
}
