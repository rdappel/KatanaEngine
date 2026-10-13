#pragma once

#include "Core/Game.h"
#include "Core/Hosting/GameHost.h"

#include <memory>
#include <string>

// Defines the C exports that KatanaEngine.WinForms' KatanaViewport control calls.
// Use it once, in one .cpp file of a game built as a DLL:
//
//     std::unique_ptr<KatanaEngine::Game> CreateHostedGame(const KatanaEngine::GameConfig& config)
//     {
//         return std::make_unique<MyGame>(config);
//     }
//
//     KATANA_DEFINE_HOST_EXPORTS(CreateHostedGame)
//
// Exported API (UTF-8 strings; the message callback runs on the game thread):
//     int  KatanaHost_Start(void* parentWindow, KatanaHostMessageCallback onMessage)  // 1 = started
//     void KatanaHost_Resize(int width, int height)
//     void KatanaHost_Post(const char* message)
//     void KatanaHost_Stop()

extern "C" typedef void(__cdecl* KatanaHostMessageCallback)(const char* message);

#define KATANA_DEFINE_HOST_EXPORTS(GameFactoryFunction)                                                         \
	namespace                                                                                                   \
	{                                                                                                           \
		KatanaEngine::GameHost& KatanaHostInstance()                                                            \
		{                                                                                                       \
			static KatanaEngine::GameHost host;                                                                 \
			return host;                                                                                        \
		}                                                                                                       \
	}                                                                                                           \
                                                                                                                \
	extern "C" __declspec(dllexport) int __cdecl KatanaHost_Start(void* parentWindow, KatanaHostMessageCallback onMessage) \
	{                                                                                                           \
		auto forward = [onMessage](const std::string& message) { if (onMessage) onMessage(message.c_str()); }; \
		return KatanaHostInstance().Start(parentWindow, GameFactoryFunction, forward) ? 1 : 0;                 \
	}                                                                                                           \
                                                                                                                \
	extern "C" __declspec(dllexport) void __cdecl KatanaHost_Resize(int width, int height)                   \
	{                                                                                                           \
		KatanaHostInstance().Resize(width, height);                                                             \
	}                                                                                                           \
                                                                                                                \
	extern "C" __declspec(dllexport) void __cdecl KatanaHost_Post(const char* message)                        \
	{                                                                                                           \
		if (message) KatanaHostInstance().Post(message);                                                        \
	}                                                                                                           \
                                                                                                                \
	extern "C" __declspec(dllexport) void __cdecl KatanaHost_Stop()                                           \
	{                                                                                                           \
		KatanaHostInstance().Stop();                                                                            \
	}
