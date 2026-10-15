#include "pch.h"
#include "FileBytes.h"

#include <fstream>

namespace KatanaEngine
{
	bool ReadAllBytes(const std::string& path, std::vector<uint8_t>& out)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file.is_open()) return false;

		const std::streamsize size = file.tellg();
		if (size < 0) return false;

		std::vector<uint8_t> bytes(static_cast<size_t>(size));
		file.seekg(0, std::ios::beg);
		if (size > 0 && !file.read(reinterpret_cast<char*>(bytes.data()), size)) return false;

		out = std::move(bytes);
		return true;
	}
}
