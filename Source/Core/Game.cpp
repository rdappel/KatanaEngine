#include "pch.h"

#include "Game.h"

// Core/Platform
#include "Platform/PlatformContext.h"
#include "Platform/PlatformColor.h"
#include "Platform/PlatformMouseCursor.h"
#include "Platform/WindowEmbedding.h"
#include "Services/EngineServices.h"

// Services (optional)
#include "Screens/ScreenManager.h"

// Core services (owned by framework)
#include "Input/InputState.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/Color.h"
#include "GameTime.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include <climits>

namespace KatanaEngine
{
	Game::Game(int argc, char** argv, const GameConfig& config)
		: m_pContext(std::make_unique<PlatformContext>())
		, m_resourceManager(config.ContentRoot)
		, m_screenManager(std::make_shared<ScreenManager>(this))
		, m_config(config)
	{
		EngineServices::Configure(m_screenManager);
		m_screenWidth = m_config.ScreenWidth;
		m_screenHeight = m_config.ScreenHeight;
		m_resourceManager.SetGame(this);  // Connect ResourceManager to Game for backend detection
	}

	Game::~Game()
	{
		Shutdown();
	}

	void Game::SetScreenSize(int width, int height)
	{
		m_screenWidth = width;
		m_screenHeight = height;

		if (!m_pContext || !m_pContext->pDisplay) return;
		al_resize_display(m_pContext->pDisplay.get(), width, height);
		UpdateScreenSizeFromDisplay();
	}

	void Game::SetTitle(const std::string& title)
	{
		m_title = title;

		if (!m_pContext || !m_pContext->pDisplay) return;
		al_set_window_title(m_pContext->pDisplay.get(), title.c_str());
	}

	void Game::SetTargetFrameRate(double fps)
	{
		m_targetFrameRate = fps;
		m_targetElapsedTime = 1.0 / fps;

		if (!m_pContext || !m_pContext->pTimer) return;
		al_set_timer_speed(m_pContext->pTimer.get(), m_targetElapsedTime);
	}

	bool Game::SetMouseCursor(MouseCursor cursor)
	{
		if (!m_pContext || !m_pContext->pDisplay)
		{
			return false;
		}

		return al_set_system_mouse_cursor(m_pContext->pDisplay.get(), ToAllegroMouseCursor(cursor));
	}

	void Game::Clear(const Color& color)
	{
		al_clear_to_color(ToAllegroColor(color));
	}

	void Game::UpdateScreenSizeFromDisplay()
	{
		if (!m_pContext || !m_pContext->pDisplay)
		{
			return;
		}

		m_screenWidth = al_get_display_width(m_pContext->pDisplay.get());
		m_screenHeight = al_get_display_height(m_pContext->pDisplay.get());
	}

	bool Game::InitializeAllegro()
	{
		// Hosted games live in a DLL: an atexit handler would run al_uninstall_system under the
		// loader lock at process exit and deadlock. GameHost uninstalls Allegro explicitly instead.
		const bool installed = m_config.ParentWindow
			? al_install_system(ALLEGRO_VERSION_INT, nullptr)
			: al_init();

		if (!installed)
		{
			std::cerr << "ERROR: Failed to initialize Allegro core library" << std::endl;
			return false;
		}

		// Log Allegro version for debugging
		uint32_t version = al_get_allegro_version();
		int major = version >> 24;
		int minor = (version >> 16) & 255;
		int revision = (version >> 8) & 255;
		int release = version & 255;
		std::cout << "Allegro Version: " << major << "." << minor << "." << revision << "." << release << std::endl;

		al_init_font_addon();
		al_init_ttf_addon();
		al_init_primitives_addon();
		al_init_image_addon();
		if (!al_install_audio())
		{
			std::cerr << "ERROR: Failed to install Allegro audio" << std::endl;
			return false;
		}
		if (!al_init_acodec_addon())
		{
			std::cerr << "ERROR: Failed to initialize Allegro audio codecs" << std::endl;
			return false;
		}
		if (!al_reserve_samples(32))
		{
			std::cerr << "ERROR: Failed to reserve Allegro audio samples" << std::endl;
			return false;
		}

		al_install_keyboard();
		al_install_mouse();

		const bool isHosted = m_config.ParentWindow != nullptr;

		const int baseDisplayFlags = isHosted
			? (ALLEGRO_WINDOWED | ALLEGRO_FRAMELESS)
			: m_config.Fullscreen
				? ALLEGRO_FULLSCREEN_WINDOW
				: (ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);

		if (isHosted)
		{
			// Match the host's client area and create the window offscreen so it never
			// flashes as a top-level window before it is embedded.
			Platform::GetParentClientSize(m_config.ParentWindow, m_screenWidth, m_screenHeight);
			al_set_new_window_position(-32000, -32000);
		}

		// Try to create display with shader support
		// On Windows, try Direct3D first (better compatibility), then OpenGL
		// On other platforms, only OpenGL is available

#ifdef _WIN32
		if (m_config.PreferDirect3D)
		{
			std::cout << "Attempting to create display with Direct3D shader support..." << std::endl;

			// First try: D3D with programmable pipeline
			al_set_new_display_flags(baseDisplayFlags | ALLEGRO_PROGRAMMABLE_PIPELINE | ALLEGRO_DIRECT3D_INTERNAL);
			al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 0, ALLEGRO_DONTCARE);
			al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 0, ALLEGRO_DONTCARE);
			al_set_new_display_option(ALLEGRO_SAMPLES, 0, ALLEGRO_DONTCARE);

			m_pContext->pDisplay.reset(al_create_display(m_screenWidth, m_screenHeight));

			if (m_pContext->pDisplay)
			{
				m_graphicsBackend = GraphicsBackend::Direct3D;
				std::cout << "SUCCESS: Display created with Direct3D shader support." << std::endl;
				std::cout << "Shader support: ENABLED (HLSL)" << std::endl;
			}
			else
			{
				std::cerr << "Direct3D with programmable pipeline failed" << std::endl;

				// Second try: D3D without programmable pipeline (legacy mode)
				std::cout << "Attempting Direct3D without programmable pipeline..." << std::endl;
				al_set_new_display_flags(baseDisplayFlags | ALLEGRO_DIRECT3D_INTERNAL);
				al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 0, ALLEGRO_DONTCARE);
				al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 0, ALLEGRO_DONTCARE);
				al_set_new_display_option(ALLEGRO_SAMPLES, 0, ALLEGRO_DONTCARE);

				m_pContext->pDisplay.reset(al_create_display(m_screenWidth, m_screenHeight));

				if (m_pContext->pDisplay)
				{
					m_graphicsBackend = GraphicsBackend::Unknown; // No shader support but D3D rendering
					std::cout << "SUCCESS: Display created with Direct3D (legacy mode, no shaders)." << std::endl;
					std::cout << "WARNING: Custom shaders will NOT work." << std::endl;
				}
				else
				{
					std::cerr << "Direct3D legacy mode also failed" << std::endl;
				}
			}
		}
#endif

		// Try OpenGL if D3D failed or not preferred
		if (!m_pContext->pDisplay)
		{
			std::cout << "Attempting to create display with OpenGL shader support..." << std::endl;
			al_set_new_display_flags(baseDisplayFlags | ALLEGRO_PROGRAMMABLE_PIPELINE);

			// Minimal OpenGL settings for better compatibility
			al_set_new_display_option(ALLEGRO_OPENGL, 1, ALLEGRO_SUGGEST);
			al_set_new_display_option(ALLEGRO_OPENGL_MAJOR_VERSION, 2, ALLEGRO_SUGGEST);
			al_set_new_display_option(ALLEGRO_OPENGL_MINOR_VERSION, 0, ALLEGRO_SUGGEST);
			al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 0, ALLEGRO_DONTCARE);
			al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 0, ALLEGRO_DONTCARE);
			al_set_new_display_option(ALLEGRO_SAMPLES, 0, ALLEGRO_DONTCARE);

			m_pContext->pDisplay.reset(al_create_display(m_screenWidth, m_screenHeight));

			if (m_pContext->pDisplay)
			{
				m_graphicsBackend = GraphicsBackend::OpenGL;
				int opengl_version = al_get_display_option(m_pContext->pDisplay.get(), ALLEGRO_OPENGL_MAJOR_VERSION);
				int opengl_minor = al_get_display_option(m_pContext->pDisplay.get(), ALLEGRO_OPENGL_MINOR_VERSION);
				std::cout << "SUCCESS: Display created with OpenGL shader support." << std::endl;
				std::cout << "OpenGL Version: " << opengl_version << "." << opengl_minor << std::endl;
				std::cout << "Shader support: ENABLED (GLSL)" << std::endl;
			}
			else
			{
				std::cerr << "OpenGL initialization failed" << std::endl;
			}
		}

		// Fallback: No shader support
		if (!m_pContext->pDisplay)
		{
			std::cerr << "WARNING: Failed to create display with shader support." << std::endl;
			std::cerr << "This may be due to:" << std::endl;
			std::cerr << "  - Outdated graphics drivers" << std::endl;
			std::cerr << "  - Insufficient OpenGL/DirectX support" << std::endl;
			std::cerr << "  - Graphics virtualization or remote desktop" << std::endl;
			std::cerr << std::endl;
			std::cerr << "Attempting fallback without shaders..." << std::endl;

			// Try again without programmable pipeline
			al_set_new_display_flags(baseDisplayFlags);
			al_set_new_display_option(ALLEGRO_OPENGL, 0, ALLEGRO_DONTCARE);
			m_pContext->pDisplay.reset(al_create_display(m_screenWidth, m_screenHeight));

			if (!m_pContext->pDisplay)
			{
				std::cerr << "ERROR: Failed to create display!" << std::endl;
				return false;
			}

			m_graphicsBackend = GraphicsBackend::Unknown;
			std::cout << "SUCCESS: Display created without shader support." << std::endl;
			std::cout << "WARNING: Custom shaders will NOT work." << std::endl;
		}

		if (isHosted)
		{
			al_set_new_window_position(INT_MAX, INT_MAX);  // restore default placement for later displays

			if (!Platform::EmbedDisplay(m_pContext->pDisplay.get(), m_config.ParentWindow))
			{
				std::cerr << "ERROR: Failed to embed display in host window" << std::endl;
				return false;
			}
		}

		UpdateScreenSizeFromDisplay();
		al_set_window_title(m_pContext->pDisplay.get(), m_title.c_str());

		m_pContext->pEventQueue.reset(al_create_event_queue());
		if (!m_pContext->pEventQueue)
		{
			std::cerr << "ERROR: Failed to create event queue" << std::endl;
			return false;
		}

		m_pContext->pTimer.reset(al_create_timer(m_targetElapsedTime));
		if (!m_pContext->pTimer)
		{
			std::cerr << "ERROR: Failed to create timer" << std::endl;
			return false;
		}

		al_register_event_source(m_pContext->pEventQueue.get(), al_get_display_event_source(m_pContext->pDisplay.get()));
		al_register_event_source(m_pContext->pEventQueue.get(), al_get_timer_event_source(m_pContext->pTimer.get()));
		al_register_event_source(m_pContext->pEventQueue.get(), al_get_keyboard_event_source());
		al_register_event_source(m_pContext->pEventQueue.get(), al_get_mouse_event_source());

		return true;
	}

	int Game::Shutdown()
	{
		EngineServices::Reset();

		if (m_isInitialized) UnloadContent();

		// Optional services
		m_screenManager->ClearImmediate();
		m_resourceManager.ClearCache();

		if (!m_pContext) return 1;

		m_pContext->pTimer.reset();
		m_pContext->pEventQueue.reset();
		m_pContext->pDisplay.reset();

		m_isInitialized = false;

		return 0;
	}

	int Game::Run()
	{
		if (!InitializeAllegro()) return 1;

		LoadContent(m_resourceManager);
		m_isInitialized = true;

		if (m_startedCallback) m_startedCallback();

		m_isRunning = true;
		m_previousTime = al_get_time();
		al_start_timer(m_pContext->pTimer.get());

		GameLoop();
		return Shutdown();
	}

	void Game::GameLoop()
	{
		bool needsRedraw = true;

		while (m_isRunning)
		{
			ALLEGRO_EVENT event;
			al_wait_for_event(m_pContext->pEventQueue.get(), &event);

			if (m_hasHostRequests.load(std::memory_order_acquire))
			{
				ProcessHostRequests();
				if (!m_isRunning) break;
			}

			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				m_isRunning = false;
			}
			else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
			{
				al_acknowledge_resize(m_pContext->pDisplay.get());
				UpdateScreenSizeFromDisplay();
			}
			else if (event.type == ALLEGRO_EVENT_TIMER)
			{
				const double currentTime = al_get_time();
				const double frameTime = currentTime - m_previousTime;
				m_previousTime = currentTime;

				m_accumulatedTime += frameTime;

				int updateCount = 0;
				const bool isRunningSlowly = m_accumulatedTime > m_targetElapsedTime * MAX_FRAME_SKIP;

				while (m_accumulatedTime >= m_targetElapsedTime && updateCount < MAX_FRAME_SKIP)
				{
					m_totalTime += m_targetElapsedTime;
					m_accumulatedTime -= m_targetElapsedTime;

					m_inputState.Update();
					HandleInput(m_inputState);

					GameTime gameTime(m_totalTime, m_targetElapsedTime, isRunningSlowly);
					Update(gameTime);

					updateCount++;
				}

				needsRedraw = true;
			}

			if (needsRedraw && al_is_event_queue_empty(m_pContext->pEventQueue.get()))
			{
				needsRedraw = false;

				Draw(m_spriteBatch);

				al_flip_display();
			}
		}
	}

	void Game::RequestExit()
	{
		std::lock_guard<std::mutex> lock(m_hostMutex);
		m_exitRequested = true;
		m_hasHostRequests.store(true, std::memory_order_release);
	}

	void Game::RequestResize(int width, int height)
	{
		if (width <= 0 || height <= 0) return;

		std::lock_guard<std::mutex> lock(m_hostMutex);
		m_requestedWidth = width;
		m_requestedHeight = height;
		m_hasHostRequests.store(true, std::memory_order_release);
	}

	void Game::PostHostMessage(const std::string& message)
	{
		std::lock_guard<std::mutex> lock(m_hostMutex);
		m_pendingHostMessages.push_back(message);
		m_hasHostRequests.store(true, std::memory_order_release);
	}

	void Game::SendHostMessage(const std::string& message) const
	{
		if (m_hostMessageCallback) m_hostMessageCallback(message);
	}

	void Game::ProcessHostRequests()
	{
		bool exitRequested = false;
		int width = 0;
		int height = 0;

		{
			std::lock_guard<std::mutex> lock(m_hostMutex);
			exitRequested = m_exitRequested;
			width = m_requestedWidth;
			height = m_requestedHeight;
			m_requestedWidth = 0;
			m_requestedHeight = 0;
			m_processingHostMessages.swap(m_pendingHostMessages);
			m_hasHostRequests.store(false, std::memory_order_release);
		}

		if (exitRequested)
		{
			m_isRunning = false;
			return;
		}

		if (width > 0 && height > 0 && m_config.ParentWindow)
		{
			Platform::ResizeEmbeddedDisplay(m_pContext->pDisplay.get(), width, height);
			UpdateScreenSizeFromDisplay();
		}

		for (const auto& message : m_processingHostMessages)
		{
			OnHostMessage(message);
		}
		m_processingHostMessages.clear();
	}
}
