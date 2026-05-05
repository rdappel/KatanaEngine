#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>

namespace KatanaEngine
{
	class Texture;
	class Font;
	class AnimationSet;
	class Effect;
	class Sound;
	class Music;
	class Game;

	class ResourceManager
	{
	public:
		explicit ResourceManager(const std::string& contentRoot);
		~ResourceManager() { m_resourceCache.clear(); }

		template<typename T>
		std::shared_ptr<T> Load(const std::string& path);

		const std::string& GetContentRoot() const { return m_contentRoot; }
		void ClearCache() { m_resourceCache.clear(); }

		void SetGame(const Game* game) { m_pGame = game; }

	private:
		std::string MakeFullPath(const std::string& path) const;
		std::string MakeCacheKey(std::type_index type, const std::string& path) const;

		std::string m_contentRoot;
		std::unordered_map<std::string, std::shared_ptr<void>> m_resourceCache;
		const Game* m_pGame = nullptr;
	};

    // Explicit template declarations
    template<> std::shared_ptr<Texture> ResourceManager::Load<Texture>(const std::string& path);
    template<> std::shared_ptr<Font> ResourceManager::Load<Font>(const std::string& path);
    template<> std::shared_ptr<AnimationSet> ResourceManager::Load<AnimationSet>(const std::string& path);
    template<> std::shared_ptr<Effect> ResourceManager::Load<Effect>(const std::string& path);
    template<> std::shared_ptr<Sound> ResourceManager::Load<Sound>(const std::string& path);
    template<> std::shared_ptr<Music> ResourceManager::Load<Music>(const std::string& path);
}
