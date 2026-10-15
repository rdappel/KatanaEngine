#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace KatanaEngine
{
	// Reads a whole file into memory. Returns false if it can't be opened or fully read.
	bool ReadAllBytes(const std::string& path, std::vector<uint8_t>& out);
}
