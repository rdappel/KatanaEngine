#include "pch.h"
#include "BinaryReader.h"

#include <cstring>

namespace KatanaEngine
{
	BinaryReader::BinaryReader(const std::vector<uint8_t>& data, size_t offset)
		: m_pData(&data)
		, m_offset(std::min(offset, data.size()))
	{
	}

	bool BinaryReader::Seek(size_t offset)
	{
		if (offset > m_pData->size()) return false;

		m_offset = offset;
		return true;
	}

	bool BinaryReader::ReadFloat(float& out)
	{
		uint32_t raw = 0;
		if (!Read(raw)) return false;

		std::memcpy(&out, &raw, sizeof(float));
		return true;
	}

	bool BinaryReader::ReadBytes(uint8_t* out, size_t count)
	{
		if (GetRemaining() < count) return false;
		if (count == 0) return true;

		std::memcpy(out, GetCurrent(), count);
		m_offset += count;
		return true;
	}

	bool BinaryReader::ReadBytes(std::vector<uint8_t>& out, size_t count)
	{
		if (GetRemaining() < count) return false;

		out.assign(GetCurrent(), GetCurrent() + count);
		m_offset += count;
		return true;
	}

	bool BinaryReader::ReadString16(std::string& out)
	{
		const size_t start = m_offset;
		uint16_t length = 0;
		if (!Read(length)) return false;

		if (GetRemaining() < length)
		{
			m_offset = start;
			return false;
		}

		out.assign(reinterpret_cast<const char*>(GetCurrent()), length);
		m_offset += length;
		return true;
	}

	bool BinaryReader::ReadString32(std::string& out)
	{
		const size_t start = m_offset;
		uint32_t length = 0;
		if (!Read(length)) return false;

		if (GetRemaining() < length)
		{
			m_offset = start;
			return false;
		}

		out.assign(reinterpret_cast<const char*>(GetCurrent()), length);
		m_offset += length;
		return true;
	}

	bool BinaryReader::ReadStringList16(std::vector<std::string>& out)
	{
		const size_t start = m_offset;
		uint16_t count = 0;
		if (!Read(count)) return false;

		std::vector<std::string> values;
		values.reserve(count);
		for (uint16_t i = 0; i < count; ++i)
		{
			std::string value;
			if (!ReadString16(value))
			{
				m_offset = start;
				return false;
			}

			values.push_back(std::move(value));
		}

		out = std::move(values);
		return true;
	}
}
