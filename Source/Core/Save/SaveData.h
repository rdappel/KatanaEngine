
#pragma once

#include <string>
#include <unordered_map>

namespace KatanaEngine
{
	class SaveData
	{
	public:
		void Set(const std::string& key, const std::string& value);
		std::string Get(const std::string& key, const std::string& defaultValue = "") const;
		bool Has(const std::string& key) const;

		void SetInt(const std::string& key, int value);
		int GetInt(const std::string& key, int defaultValue = 0) const;

		void SetBool(const std::string& key, bool value);
		bool GetBool(const std::string& key, bool defaultValue = false) const;

		const std::unordered_map<std::string, std::string>& GetValues() const;

	private:
		std::unordered_map<std::string, std::string> m_values;
	};

	class ISaveable
	{
	public:
		virtual ~ISaveable() = default;

		virtual void Save(SaveData& data) const = 0;
		virtual void Load(const SaveData& data) = 0;
	};
}