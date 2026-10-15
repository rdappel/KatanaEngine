
#include "pch.h"
#include "SaveData.h"

// #include <cstdlib>

namespace KatanaEngine
{
	void SaveData::Set(const std::string& key, const std::string& value)
	{
		m_values[key] = value;
	}

	std::string SaveData::Get(const std::string& key, const std::string& defaultValue) const
	{
		auto iter = m_values.find(key);
		return iter != m_values.end() ? iter->second : defaultValue;
	}

	bool SaveData::Has(const std::string& key) const
	{
		return m_values.find(key) != m_values.end();
	}

	void SaveData::SetInt(const std::string& key, int value)
	{
		Set(key, std::to_string(value));
	}

	int SaveData::GetInt(const std::string& key, int defaultValue) const
	{
		auto iter = m_values.find(key);
		if (iter == m_values.end()) return defaultValue;

		try
		{
			return std::stoi(iter->second);
		}
		catch (...)
		{
			return defaultValue;
		}
	}

	void SaveData::SetBool(const std::string& key, bool value)
	{
		Set(key, value ? "true" : "false");
	}

	bool SaveData::GetBool(const std::string& key, bool defaultValue) const
	{
		auto iter = m_values.find(key);

		if (iter == m_values.end()) return defaultValue;

		return iter->second == "true" || iter->second == "1";
	}

	const std::unordered_map<std::string, std::string>& SaveData::GetValues() const
	{
		return m_values;
	}
}