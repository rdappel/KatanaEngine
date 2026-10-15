#pragma once

#include <cstdint>
#include <vector>

namespace KatanaEngine
{
	// Decodes standard Base64 (RFC 4648 alphabet). Characters outside the alphabet,
	// including padding and whitespace, are skipped.
	std::vector<uint8_t> DecodeBase64(const std::vector<uint8_t>& encoded);
}
