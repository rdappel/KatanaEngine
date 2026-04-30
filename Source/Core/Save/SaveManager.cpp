
#include "pch.h"
#include "SaveManager.h"

#include <algorithm>

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
}