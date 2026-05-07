#include "pch.h"

#include "Screen.h"
#include "ScreenManager.h"

namespace KatanaEngine
{
	Screen::Screen(bool show)
     : m_isDrawBlocking(true)
		, m_isUpdateBlocking(true)
		, m_isInputBlocking(true)
		, m_showOnAdd(show)
		, m_isShown(false)
		, m_isExiting(false)
		, m_isRemoved(false)
	{ }

	void Screen::Show()
	{
		BeginShow();
	}

	void Screen::Exit()
	{
		if (!BeginExit())
		{
			return;
		}

		RemoveFromManager();
	}

	bool Screen::BeginShow()
	{
		if (m_isShown)
		{
			return false;
		}

		m_isShown = true;
		if (OnShow)
		{
			OnShow();
		}

		return true;
	}

	bool Screen::BeginExit()
	{
		if (m_isExiting)
		{
			return false;
		}

		m_isExiting = true;
		if (OnExit)
		{
			OnExit();
		}

		return true;
	}

	void Screen::RemoveFromManager()
	{
       auto manager = m_pScreenManager.lock();
		if (manager)
		{
           manager->RemoveScreen(this);
		}
	}

	void Screen::NotifyRemoved()
	{
		if (m_isRemoved)
		{
			return;
		}

		m_isRemoved = true;
		if (OnRemoved)
		{
			OnRemoved();
		}
	}
}
