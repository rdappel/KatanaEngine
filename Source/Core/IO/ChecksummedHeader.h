#pragma once

#include <cstddef>
#include <cstdint>

namespace KatanaEngine
{
	class BinaryReader;

	// The 10-byte header that starts KE.Editor's binary exports (.kemapbin, .kecharbin, item
	// database): magic (uint32), version (uint16), then a CRC-32 of every byte after the header.
	struct ChecksummedHeader
	{
		static constexpr size_t Size = 10;

		uint32_t Magic = 0;
		uint16_t Version = 0;
		uint32_t StoredCrc = 0;
		uint32_t ComputedCrc = 0;

		bool IsChecksumValid() const { return StoredCrc == ComputedCrc; }
	};

	// Reads the header at the reader's position and computes the CRC of everything after it.
	// Returns false only if the header itself is truncated; callers check Magic, Version and
	// IsChecksumValid in whatever order and with whatever messages suit their format.
	bool ReadChecksummedHeader(BinaryReader& reader, ChecksummedHeader& header);
}
