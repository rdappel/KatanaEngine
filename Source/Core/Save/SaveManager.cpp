
#include "pch.h"
#include "SaveManager.h"

#include <algorithm>
#include <fstream>

namespace KatanaEngine
{
	void SaveManager::Register(ISaveable* saveable)
	{
		if (!saveable) return;

		auto iter = std::find(m_saveables.begin(), m_saveables.end(), saveable);

		if (iter != m_saveables.end()) return;

		m_saveables.push_back(saveable);
	}

	void SaveManager::Unregister(ISaveable* saveable)
	{
		m_saveables.erase(
			std::remove(m_saveables.begin(), m_saveables.end(), saveable),
			m_saveables.end()
		);
	}

	SaveData SaveManager::CreateSave() const
	{
		SaveData data;

		for (auto* saveable : m_saveables)
		{
			if (saveable) saveable->Save(data);
		}

		return data;
	}

	void SaveManager::LoadSave(const SaveData& data)
	{
		for (auto* saveable : m_saveables)
		{
			if (saveable) saveable->Load(data);
		}
	}

	bool SaveManager::SaveToFile(const std::string& path) const
	{
		std::ofstream file(path);
		if (!file.is_open()) return false;

		const SaveData data = CreateSave();
		for (const auto& [key, value] : data.GetValues())
			file << key << "=" << value << "\n";

		std::cout << "[Save] Saved " << data.GetValues().size() << " entries to " << path << "\n";
		return true;
	}

	bool SaveManager::LoadFromFile(const std::string& path)
	{
		std::ifstream file(path);
		if (!file.is_open()) return false;

		SaveData data;
		std::string line;
		while (std::getline(file, line))
		{
			const size_t eq = line.find('=');
			if (eq == std::string::npos) continue;
			data.Set(line.substr(0, eq), line.substr(eq + 1));
		}

		LoadSave(data);
		return true;
	}
}