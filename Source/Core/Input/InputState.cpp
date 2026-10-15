#include "pch.h"

#include "InputState.h"
#include "Core/Platform/PlatformInput.h"

#include <allegro5/allegro.h>

namespace KatanaEngine
{
	InputState::InputState()
		: m_mousePosition(0, 0)
	{
		m_currentKeyState.fill(false);
		m_previousKeyState.fill(false);
		m_currentMouseState.fill(false);
		m_previousMouseState.fill(false);
	}

	void InputState::Update()
	{
		// Copy current state to previous
		m_previousKeyState = m_currentKeyState;
		m_previousMouseState = m_currentMouseState;
		m_previousMouseWheelZ = m_mouseWheelZ;

		// Get current keyboard state from Allegro
		ALLEGRO_KEYBOARD_STATE keyboardState;
		al_get_keyboard_state(&keyboardState);

		// Update current state for all keys
		for (int i = 0; i < KEY_COUNT; ++i)
		{
			Keys key = static_cast<Keys>(i);
			int allegroKey = ToAllegroKey(key);

			if (allegroKey >= 0)
				m_currentKeyState[i] = al_key_down(&keyboardState, allegroKey);
			else
				m_currentKeyState[i] = false;
		}

		// Get current mouse state from Allegro
		ALLEGRO_MOUSE_STATE mouseState;
		al_get_mouse_state(&mouseState);

		// Update mouse position
		m_mousePosition.X = static_cast<float>(mouseState.x);
		m_mousePosition.Y = static_cast<float>(mouseState.y);
		m_mouseWheelZ = mouseState.z;

		// Update mouse button states
		m_currentMouseState[static_cast<int>(MouseButton::LEFT)] = (mouseState.buttons & 1) != 0;
		m_currentMouseState[static_cast<int>(MouseButton::RIGHT)] = (mouseState.buttons & 2) != 0;
		m_currentMouseState[static_cast<int>(MouseButton::MIDDLE)] = (mouseState.buttons & 4) != 0;
	}

	bool InputState::IsKeyDown(Keys key) const
	{
		int index = static_cast<int>(key);
		if (index < 0 || index >= KEY_COUNT) return false;
		return m_currentKeyState[index];
	}

	bool InputState::IsKeyUp(Keys key) const
	{
		return !IsKeyDown(key);
	}

	bool InputState::IsKeyPressed(Keys key) const
	{
		int index = static_cast<int>(key);
		if (index < 0 || index >= KEY_COUNT) return false;
		return m_currentKeyState[index] && !m_previousKeyState[index];
	}

	bool InputState::IsKeyReleased(Keys key) const
	{
		int index = static_cast<int>(key);
		if (index < 0 || index >= KEY_COUNT) return false;
		return !m_currentKeyState[index] && m_previousKeyState[index];
	}

	bool InputState::IsAnyKeyPressed(Keys& pressedKey) const
	{
		for (int i = 0; i < KEY_COUNT; ++i)
		{
			if (m_currentKeyState[i] && !m_previousKeyState[i])
			{
				pressedKey = static_cast<Keys>(i);
				return true;
			}
		}
		return false;
	}

	bool InputState::IsAnyKeyPressed() const
	{
		Keys ignore;
		return IsAnyKeyPressed(ignore);
	}

	bool InputState::IsAnyKeyReleased(Keys& releasedKey) const
	{
		for (int i = 0; i < KEY_COUNT; ++i)
		{
			if (!m_currentKeyState[i] && m_previousKeyState[i])
			{
				releasedKey = static_cast<Keys>(i);
				return true;
			}
		}
		return false;
	}

	bool InputState::IsAnyKeyReleased() const
	{
		Keys ignore;
		return IsAnyKeyReleased(ignore);
	}

	bool InputState::IsMouseButtonDown(MouseButton button) const
	{
		int index = static_cast<int>(button);
		if (index < 0 || index >= MOUSE_BUTTON_COUNT) return false;
		return m_currentMouseState[index];
	}

	bool InputState::IsMouseButtonUp(MouseButton button) const
	{
		return !IsMouseButtonDown(button);
	}

	bool InputState::IsMouseButtonPressed(MouseButton button) const
	{
		int index = static_cast<int>(button);
		if (index < 0 || index >= MOUSE_BUTTON_COUNT) return false;
		return m_currentMouseState[index] && !m_previousMouseState[index];
	}

	bool InputState::IsMouseButtonReleased(MouseButton button) const
	{
		int index = static_cast<int>(button);
		if (index < 0 || index >= MOUSE_BUTTON_COUNT) return false;
		return !m_currentMouseState[index] && m_previousMouseState[index];
	}

	bool InputState::IsAnyMouseButtonPressed(MouseButton& pressedButton) const
	{
		for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i)
		{
			if (m_currentMouseState[i] && !m_previousMouseState[i])
			{
				pressedButton = static_cast<MouseButton>(i);
				return true;
			}
		}
		return false;
	}

	bool InputState::IsAnyMouseButtonPressed() const
	{
		MouseButton ignore;
		return IsAnyMouseButtonPressed(ignore);
	}

	bool InputState::IsAnyMouseButtonReleased(MouseButton& releasedButton) const
	{
		for (int i = 0; i < MOUSE_BUTTON_COUNT; ++i)
		{
			if (!m_currentMouseState[i] && m_previousMouseState[i])
			{
				releasedButton = static_cast<MouseButton>(i);
				return true;
			}
		}
		return false;
	}

	bool InputState::IsAnyMouseButtonReleased() const
	{
		MouseButton ignore;
		return IsAnyMouseButtonReleased(ignore);
	}
}
