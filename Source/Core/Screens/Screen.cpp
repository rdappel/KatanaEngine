#include "pch.h"

#include "Screen.h"

namespace KatanaEngine
{
	Screen::Screen()
		: m_pScreenManager(nullptr)
		, m_isDrawBlocking(true)
		, m_isUpdateBlocking(true)
		, m_isInputBlocking(true)
	{ }
}
