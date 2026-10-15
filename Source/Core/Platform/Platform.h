#pragma once

#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace KatanaEngine
{
	class Platform
	{
	public:
		static std::string GetExecutableDirectory()
		{
#ifdef _WIN32
			char buffer[MAX_PATH];
			GetModuleFileNameA(nullptr, buffer, MAX_PATH);
			std::string path(buffer);

			size_t lastSlash = path.find_last_of("\\/");
			if (lastSlash == std::string::npos) return path;
			return path.substr(0, lastSlash + 1);
#else
			return "./";
#endif
		}
	};
}
