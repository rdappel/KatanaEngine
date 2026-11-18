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

		// Every 4 alphabet characters give at most 3 bytes, so this is never too small. Sizing once and walking
		// both buffers with pointers avoids a push_back and an iterator check per byte, which are slow in Debug.
		std::vector<uint8_t> decoded(encoded.size() * 3 / 4 + 3);
		uint8_t* pOutput = decoded.data();
		const uint8_t* pInput = encoded.data();
		const uint8_t* const pInputEnd = pInput + encoded.size();

		// Unsigned so the shift wraps instead of overflowing; only the low bits are ever used.
		uint32_t value = 0;
		int bits = -8;
		for (; pInput != pInputEnd; ++pInput)
		{
			const int8_t sextet = lookup[*pInput];
			if (sextet == -1) continue;

			value = (value << 6) | static_cast<uint32_t>(sextet);
			bits += 6;
			if (bits < 0) continue;

			*pOutput++ = static_cast<uint8_t>((value >> bits) & 0xFF);
			bits -= 8;
		}

		// Trim to the bytes written. Only a few bytes of capacity are left over (more if the input had characters
		// to skip), so there is no shrink_to_fit, which would copy the whole buffer to free them.
		decoded.resize(static_cast<size_t>(pOutput - decoded.data()));
		return decoded;
	}
}
