#include "pch.h"

#include "GameHost.h"
#include "Core/Game.h"

#include <allegro5/allegro.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace KatanaEngine
{
	namespace
	{
		// Waits until done() is true. On Windows, keeps servicing messages *sent* to this
		// thread (not posted ones), because the embedded child window lives on another
		// thread and synchronously notifies its parent while being created or destroyed.
		template<typename TPredicate>
		void WaitServicingSentMessages(TPredicate done)
		{
			while (!done())
			{
#ifdef _WIN32
				MsgWaitForMultipleObjects(0, nullptr, FALSE, 10, QS_SENDMESSAGE);
				MSG message;
				PeekMessage(&message, nullptr, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
#else
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
#endif
			}
		}
	}

	GameHost::~GameHost()
	{
		// Stop() must be called explicitly: joining here could run under the loader lock
		// when GameHost is a global in a DLL, which deadlocks.
		if (m_thread.joinable()) m_thread.detach();
	}

	bool GameHost::Start(void* parentWindow, GameFactory factory, MessageCallback onMessage)
	{
		if (m_thread.joinable() || !parentWindow || !factory) return false;

		m_state = State::Starting;
		m_threadFinished = false;
		m_thread = std::thread(&GameHost::RunGameThread, this, parentWindow, std::move(factory), std::move(onMessage));

		WaitServicingSentMessages([this] { return m_state.load() != State::Starting; });

		if (m_state.load() == State::Running) return true;

		Stop();
		return false;
	}

	void GameHost::Resize(int width, int height)
	{
		std::lock_guard<std::mutex> lock(m_gameMutex);
		if (m_pGame) m_pGame->RequestResize(width, height);
	}

	void GameHost::Post(const std::string& message)
	{
		std::lock_guard<std::mutex> lock(m_gameMutex);
		if (m_pGame) m_pGame->PostHostMessage(message);
	}

	void GameHost::Stop()
	{
		{
			std::lock_guard<std::mutex> lock(m_gameMutex);
			if (m_pGame) m_pGame->RequestExit();
		}

		if (!m_thread.joinable()) return;

		WaitServicingSentMessages([this] { return m_threadFinished.load(); });
		m_thread.join();
		m_state = State::Stopped;
	}

	void GameHost::RunGameThread(void* parentWindow, GameFactory factory, MessageCallback onMessage)
	{
		GameConfig config;
		config.ParentWindow = parentWindow;

		std::unique_ptr<Game> game = factory(config);
		if (game)
		{
			game->SetHostMessageCallback(std::move(onMessage));
			game->SetStartedCallback([this] { m_state = State::Running; });

			{
				std::lock_guard<std::mutex> lock(m_gameMutex);
				m_pGame = game.get();
			}

			game->Run();

			{
				std::lock_guard<std::mutex> lock(m_gameMutex);
				m_pGame = nullptr;
			}

			game.reset();
		}

		// Hosted games install Allegro without an atexit handler (see Game::InitializeAllegro),
		// so uninstall here, after every Allegro resource the game owned is gone.
		al_uninstall_system();

		if (m_state.load() == State::Starting) m_state = State::Failed;
		m_threadFinished = true;
	}
}
