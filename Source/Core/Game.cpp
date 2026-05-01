#include "pch.h"

#include "Game.h"

// Core/Platform
#include "Platform/PlatformContext.h"
#include "Platform/PlatformColor.h"

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

namespace KatanaEngine
{
	Game::Game(int argc, char** argv, const GameConfig& config)
		: m_pContext(std::make_unique<PlatformContext>())
		, m_resourceManager(config.ContentRoot)
		, m_screenManager(this)
		, m_config(config)
	{
		m_screenWidth = m_config.ScreenWidth;
		m_screenHeight = m_config.ScreenHeight;
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

	void Game::Clear(const Color& color)
	{
		al_clear_to_color(ToAllegroColor(color));
	}

	bool Game::InitializeAllegro()
	{
		if (!al_init()) 
		{
			std::cerr << "ERROR: Failed to initialize Allegro core library" << std::endl;
			return false;
		}

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

		// Set display flags to allow window resizing and programmable shaders
		al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_PROGRAMMABLE_PIPELINE);

		// Request OpenGL 2.0+ for shader support
		al_set_new_display_option(ALLEGRO_OPENGL, 1, ALLEGRO_SUGGEST);
		al_set_new_display_option(ALLEGRO_OPENGL_MAJOR_VERSION, 2, ALLEGRO_SUGGEST);
		al_set_new_display_option(ALLEGRO_OPENGL_MINOR_VERSION, 0, ALLEGRO_SUGGEST);
		al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 24, ALLEGRO_SUGGEST);
		al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
		al_set_new_display_option(ALLEGRO_SAMPLES, 4, ALLEGRO_SUGGEST);

		std::cout << "Attempting to create display with programmable pipeline (OpenGL 2.0+)..." << std::endl;
		m_pContext->pDisplay.reset(al_create_display(m_screenWidth, m_screenHeight));

		if (!m_pContext->pDisplay)
		{
			std::cerr << "WARNING: Failed to create display with programmable pipeline." << std::endl;
			std::cerr << "This may be due to:" << std::endl;
			std::cerr << "  - Outdated graphics drivers" << std::endl;
			std::cerr << "  - Insufficient OpenGL support (need OpenGL 2.0+)" << std::endl;
			std::cerr << "  - Graphics virtualization or remote desktop" << std::endl;
			std::cerr << std::endl;
			std::cerr << "Attempting fallback without shaders..." << std::endl;

			// Try again without programmable pipeline
			al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
			al_set_new_display_option(ALLEGRO_OPENGL, 0, ALLEGRO_DONTCARE);
			m_pContext->pDisplay.reset(al_create_display(m_screenWidth, m_screenHeight));

			if (!m_pContext->pDisplay)
			{
				std::cerr << "ERROR: Failed to create display even without programmable pipeline!" << std::endl;
				return false;
			}

			std::cout << "SUCCESS: Display created without shader support." << std::endl;
			std::cout << "WARNING: Custom shaders will NOT work in this mode." << std::endl;
		}
		else
		{
			// Display created successfully with programmable pipeline
			int opengl_version = al_get_display_option(m_pContext->pDisplay.get(), ALLEGRO_OPENGL_MAJOR_VERSION);
			int opengl_minor = al_get_display_option(m_pContext->pDisplay.get(), ALLEGRO_OPENGL_MINOR_VERSION);
			std::cout << "SUCCESS: Display created with programmable pipeline support." << std::endl;
			std::cout << "OpenGL Version: " << opengl_version << "." << opengl_minor << std::endl;
			std::cout << "Shader support: ENABLED" << std::endl;
		}

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
		if (m_isInitialized) UnloadContent();

		// Optional services
		m_screenManager.ClearImmediate();
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

			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				m_isRunning = false;
			}
			else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
			{
				m_screenWidth = event.display.width;
				m_screenHeight = event.display.height;
				al_acknowledge_resize(m_pContext->pDisplay.get());
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
}
