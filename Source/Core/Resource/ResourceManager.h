#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <vector>

namespace KatanaEngine
{
	class Texture;
	class Font;
	class AnimationSet;
	class Curve;
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

		// Creates a texture from encoded image bytes (a PNG read from a content file, for example) and caches it
		// under cacheKey, in the same cache as Load<Texture>: a later call with the same key, or Load<Texture> with
		// it as the path, returns the cached texture without reading the bytes. Keys share one namespace with
		// content paths, so choose one that can't be a real path. extension picks the image format (".png").
		// bytes are only read during the call; the texture doesn't keep them. Returns nullptr if they don't
		// decode, and nothing is cached then.
		std::shared_ptr<Texture> LoadTextureFromMemory(
			const std::string& cacheKey,
			const std::vector<uint8_t>& bytes,
			const std::string& extension = ".png");

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
	template<>
	std::shared_ptr<Texture> ResourceManager::Load<Texture>(const std::string& path);
	template<>
	std::shared_ptr<Font> ResourceManager::Load<Font>(const std::string& path);
	template<>
	std::shared_ptr<AnimationSet> ResourceManager::Load<AnimationSet>(const std::string& path);
	template<>
	std::shared_ptr<Curve> ResourceManager::Load<Curve>(const std::string& path);
	template<>
	std::shared_ptr<Effect> ResourceManager::Load<Effect>(const std::string& path);
	template<>
	std::shared_ptr<Sound> ResourceManager::Load<Sound>(const std::string& path);
	template<>
	std::shared_ptr<Music> ResourceManager::Load<Music>(const std::string& path);
}
