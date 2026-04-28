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

namespace KatanaEngine
{
	Game::Game(int argc, char** argv, const GameConfig& config)
		: m_pContext(std::make_unique<PlatformContext>())
		, m_resourceManager(config.ContentRoot)
		, m_config(config)
	{
		m_screenWidth = m_config.ScreenWidth;
		m_screenHeight = m_config.ScreenHeight;
	}

	Game::~Game()
	{
		Shutdown();
	}

	void Game::InitializeServices()
	{
		if (!m_config.UseScreenManager) return;
		m_pScreenManager = std::make_unique<ScreenManager>(*this, m_resourceManager);
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
		if (!al_init()) return false;

		al_init_font_addon();
		al_init_ttf_addon();
		al_init_primitives_addon();
		al_init_image_addon();

	al_install_keyboard();
	al_install_mouse();

	// Set display flags to allow window resizing and programmable shaders
	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_PROGRAMMABLE_PIPELINE);

	m_pContext->pDisplay.reset(al_create_display(m_screenWidth, m_screenHeight));
	if (!m_pContext->pDisplay) return false;

		al_set_window_title(m_pContext->pDisplay.get(), m_title.c_str());

		m_pContext->pEventQueue.reset(al_create_event_queue());
		if (!m_pContext->pEventQueue) return false;

		m_pContext->pTimer.reset(al_create_timer(m_targetElapsedTime));
		if (!m_pContext->pTimer) return false;

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
		m_pScreenManager.reset();
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

		InitializeServices();
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
