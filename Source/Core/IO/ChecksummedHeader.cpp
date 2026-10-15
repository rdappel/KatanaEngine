#include "pch.h"
#include "ChecksummedHeader.h"

#include "BinaryReader.h"
#include "Crc32.h"

namespace KatanaEngine
{
	bool ReadChecksummedHeader(BinaryReader& reader, ChecksummedHeader& header)
	{
		const bool isComplete =
			reader.Read(header.Magic) && reader.Read(header.Version) && reader.Read(header.StoredCrc);
		if (!isComplete) return false;

		header.ComputedCrc = ComputeCrc32(reader.GetCurrent(), reader.GetRemaining());
		return true;
	}
}
