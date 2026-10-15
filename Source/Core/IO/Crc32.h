#pragma once

#include <cstddef>
#include <cstdint>

namespace KatanaEngine
{
	// Standard CRC-32 (IEEE 802.3, reflected polynomial 0xEDB88320), as used by zlib and PNG.
	uint32_t ComputeCrc32(const uint8_t* data, size_t length);
}
