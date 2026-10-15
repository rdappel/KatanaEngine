#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

namespace KatanaEngine
{
	// Reads little-endian values from an in-memory byte buffer.
	// Every read returns false, and leaves the offset unchanged, when too few bytes remain.
	// The buffer must outlive the reader.
	class BinaryReader
	{
	public:
		explicit BinaryReader(const std::vector<uint8_t>& data, size_t offset = 0);

		size_t GetOffset() const { return m_offset; }
		size_t GetRemaining() const { return m_pData->size() - m_offset; }
		const uint8_t* GetCurrent() const { return m_pData->data() + m_offset; }

		// Moves to an absolute offset. Returns false, and leaves the offset unchanged, past the end.
		bool Seek(size_t offset);

		template<typename T>
		bool Read(T& out)
		{
			static_assert(
				std::is_integral<T>::value && !std::is_same<T, bool>::value,
				"BinaryReader::Read supports integer types only");

			using Unsigned = std::make_unsigned_t<T>;
			if (GetRemaining() < sizeof(T)) return false;

			Unsigned value = 0;
			for (size_t i = 0; i < sizeof(T); ++i)
			{
				const Unsigned byte = (*m_pData)[m_offset + i];
				value = static_cast<Unsigned>(value | static_cast<Unsigned>(byte << (8 * i)));
			}

			out = static_cast<T>(value);
			m_offset += sizeof(T);
			return true;
		}

		bool ReadFloat(float& out);
		bool ReadBytes(uint8_t* out, size_t count);
		bool ReadBytes(std::vector<uint8_t>& out, size_t count);

		// String with a uint16 byte-length prefix.
		bool ReadString16(std::string& out);

		// String with a uint32 byte-length prefix.
		bool ReadString32(std::string& out);

		// uint16 count, then that many ReadString16 strings.
		bool ReadStringList16(std::vector<std::string>& out);

	private:
		const std::vector<uint8_t>* m_pData = nullptr;
		size_t m_offset = 0;
	};
}
