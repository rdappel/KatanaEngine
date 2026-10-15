#include "pch.h"
#include "Crc32.h"

#include <array>

namespace KatanaEngine
{
	namespace
	{
		std::array<uint32_t, 256> BuildCrc32Table()
		{
			std::array<uint32_t, 256> table{};
			for (uint32_t i = 0; i < 256; ++i)
			{
				uint32_t value = i;
				for (int bit = 0; bit < 8; ++bit) value = (value & 1) ? (0xEDB88320u ^ (value >> 1)) : (value >> 1);

				table.at(i) = value;
			}

			return table;
		}
	}

	uint32_t ComputeCrc32(const uint8_t* data, size_t length)
	{
		static const std::array<uint32_t, 256> table = BuildCrc32Table();

		uint32_t crc = 0xFFFFFFFFu;
		for (size_t i = 0; i < length; ++i) crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);

		return crc ^ 0xFFFFFFFFu;
	}
}
