#pragma once

#include "Core/Math/Vector2.h"
#include "Core/Input/MouseCursor.h"
#include "Core/Screens/ScreenManager.h"
#include "Core/Game.h"

#include <memory>

namespace KatanaEngine
{
	namespace EngineServices
	{
		inline std::weak_ptr<ScreenManager>& ScreenManagerRef()
		{
			static std::weak_ptr<ScreenManager> g_screenManager;
			return g_screenManager;
		}

		inline void Configure(const std::shared_ptr<ScreenManager>& screenManager)
		{
			ScreenManagerRef() = screenManager;
		}

		inline void Reset()
		{
			ScreenManagerRef().reset();
		}

		namespace Window
		{
			inline int GetWidth()
			{
				auto screenManager = ScreenManagerRef().lock();
				if (!screenManager) return 0;
				return screenManager->GetGame().GetScreenWidth();
			}

			inline int GetHeight()
			{
				auto screenManager = ScreenManagerRef().lock();
				if (!screenManager) return 0;
				return screenManager->GetGame().GetScreenHeight();
			}

			inline Vector2 GetSize()
			{
				auto screenManager = ScreenManagerRef().lock();
				if (!screenManager) return Vector2::ZERO;
				return screenManager->GetGame().GetScreenSize();
			}
		}

		namespace Cursor
		{
			inline bool Set(MouseCursor cursor)
			{
				auto screenManager = ScreenManagerRef().lock();
				if (!screenManager) return false;
				return screenManager->GetGame().SetMouseCursor(cursor);
			}
		}
	}
}
