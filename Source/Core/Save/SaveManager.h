
#pragma once

#include <vector>
#include "SaveData.h"

namespace KatanaEngine
{
	class SaveManager
	{

	public:
		void Register(ISaveable* saveable);
		void Unregister(ISaveable* saveable);

		SaveData CreateSave() const;
		void LoadSave(const SaveData& data);

		bool SaveToFile(const std::string& path) const;
		bool LoadFromFile(const std::string& path);

	private:
		std::vector<ISaveable*> m_saveables;
	};
}