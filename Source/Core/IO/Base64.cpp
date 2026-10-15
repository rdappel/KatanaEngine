#include "pch.h"
#include "Base64.h"

#include <array>

namespace KatanaEngine
{
	namespace
	{
		std::array<int8_t, 256> BuildBase64Lookup()
		{
			const char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			std::array<int8_t, 256> lookup{};
			lookup.fill(-1);
			for (int i = 0; i < 64; ++i) lookup.at(static_cast<uint8_t>(alphabet[i])) = static_cast<int8_t>(i);

			return lookup;
		}
	}

	std::vector<uint8_t> DecodeBase64(const std::vector<uint8_t>& encoded)
	{
		static const std::array<int8_t, 256> lookup = BuildBase64Lookup();

		std::vector<uint8_t> decoded;
		decoded.reserve(encoded.size() * 3 / 4);

		// Unsigned so the shift wraps instead of overflowing; only the low bits are ever used.
		uint32_t value = 0;
		int bits = -8;
		for (const uint8_t character : encoded)
		{
			const int8_t sextet = lookup[character];
			if (sextet == -1) continue;

			value = (value << 6) | static_cast<uint32_t>(sextet);
			bits += 6;
			if (bits < 0) continue;

			decoded.push_back(static_cast<uint8_t>((value >> bits) & 0xFF));
			bits -= 8;
		}

		return decoded;
	}
}
