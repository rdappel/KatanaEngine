#include "pch.h"

#include "ResourceManager.h"

#include "../Platform/Platform.h"
#include "Core/Resource/Texture.h"
#include "Core/Platform/PlatformTexture.h"
#include "Core/Resource/Font.h"
#include "Core/Platform/PlatformFont.h"
#include "Core/Resource/AnimationSet.h"
#include "Core/Resource/Effect.h"
#include "Core/Resource/Sound.h"
#include "Core/Resource/Music.h"

#include <allegro5/allegro_audio.h>

#include <cassert>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

namespace KatanaEngine
{
    namespace
    {
        void LogResourceError(const std::string& message)
        {
            std::cerr << message << "\n";

#ifdef _WIN32
            OutputDebugStringA(message.c_str());
            OutputDebugStringA("\n");
#endif
        }
    }

    ResourceManager::ResourceManager(const std::string& contentRoot)
        : m_contentRoot(Platform::GetExecutableDirectory() + contentRoot)
    { }

    std::string ResourceManager::MakeFullPath(const std::string& path) const
    {
        if (m_contentRoot.empty())
        {
            return path;
        }

        // Check if contentRoot ends with separator
        char lastChar = m_contentRoot.back();
        if (lastChar == '/' || lastChar == '\\')
        {
            return m_contentRoot + path;
        }
        return m_contentRoot + "/" + path;
    }

    std::string ResourceManager::MakeCacheKey(std::type_index type, const std::string& path) const
    {
        return std::string(type.name()) + ":" + path;
    }

    template<>
    std::shared_ptr<Texture> ResourceManager::Load<Texture>(const std::string& path)
    {
        std::string cacheKey = MakeCacheKey(typeid(Texture), path);

        auto it = m_resourceCache.find(cacheKey);
        if (it != m_resourceCache.end())
        {
            return std::static_pointer_cast<Texture>(it->second);
        }

        std::string fullPath = MakeFullPath(path);

        auto texture = std::shared_ptr<Texture>(new Texture());
        texture->SetBitmap(al_load_bitmap(fullPath.c_str()));

        if (!texture->IsValid())
        {
            return nullptr;
        }

        m_resourceCache[cacheKey] = texture;
        return texture;
    }

    template<>
    std::shared_ptr<Font> ResourceManager::Load<Font>(const std::string& path)
    {
        // Cache key includes the full path with size (e.g., "Fonts/Arial.ttf@16")
        std::string cacheKey = MakeCacheKey(typeid(Font), path);

        auto it = m_resourceCache.find(cacheKey);
        if (it != m_resourceCache.end())
        {
            return std::static_pointer_cast<Font>(it->second);
        }

        // Parse path for size: "path/to/font.ttf@16"
        std::string filePath = path;
        int fontSize = 12; // Default size

        size_t atPos = path.rfind('@');
        if (atPos != std::string::npos)
        {
            filePath = path.substr(0, atPos);
            std::string sizeStr = path.substr(atPos + 1);
            fontSize = std::stoi(sizeStr);
        }

        std::string fullPath = MakeFullPath(filePath);

        auto font = std::shared_ptr<Font>(new Font());
        font->m_pData->pFont = al_load_ttf_font(fullPath.c_str(), fontSize, 0);
        font->m_size = fontSize;

        if (!font->IsValid())
        {
            return nullptr;
        }

        m_resourceCache[cacheKey] = font;
        return font;
    }

    template<>
    std::shared_ptr<AnimationSet> ResourceManager::Load<AnimationSet>(const std::string& path)
    {
        std::string cacheKey = MakeCacheKey(typeid(AnimationSet), path);

        auto it = m_resourceCache.find(cacheKey);
        if (it != m_resourceCache.end())
        {
            return std::static_pointer_cast<AnimationSet>(it->second);
        }

        std::string fullPath = MakeFullPath(path);

        auto animSet = std::shared_ptr<AnimationSet>(new AnimationSet());
        if (!animSet->LoadFromFile(fullPath))
        {
            return nullptr;
        }

        m_resourceCache[cacheKey] = animSet;
        return animSet;
    }

    template<>
    std::shared_ptr<Effect> ResourceManager::Load<Effect>(const std::string& path)
    {
        std::string cacheKey = MakeCacheKey(typeid(Effect), path);

        auto it = m_resourceCache.find(cacheKey);
        if (it != m_resourceCache.end())
        {
            return std::static_pointer_cast<Effect>(it->second);
        }

        std::string fullPath = MakeFullPath(path);

        auto effect = std::shared_ptr<Effect>(new Effect());
        if (!effect->LoadFromFile(fullPath))
        {
            LogResourceError("ResourceManager: failed to load effect: " + fullPath);
            assert(false && "ResourceManager failed to load Effect. See Output window for shader compile details.");
            return nullptr;
        }

        m_resourceCache[cacheKey] = effect;
        return effect;
    }

    template<>
    std::shared_ptr<Sound> ResourceManager::Load<Sound>(const std::string& path)
    {
        std::string cacheKey = MakeCacheKey(typeid(Sound), path);

        auto it = m_resourceCache.find(cacheKey);
        if (it != m_resourceCache.end())
        {
            return std::static_pointer_cast<Sound>(it->second);
        }

        std::string fullPath = MakeFullPath(path);

        auto sound = std::shared_ptr<Sound>(new Sound());
        sound->SetSample(al_load_sample(fullPath.c_str()));

        if (!sound->IsValid())
        {
            std::cerr << "ResourceManager: failed to load sound: " << fullPath << "\n";
            return nullptr;
        }

        m_resourceCache[cacheKey] = sound;
        return sound;
    }

    template<>
    std::shared_ptr<Music> ResourceManager::Load<Music>(const std::string& path)
    {
        std::string cacheKey = MakeCacheKey(typeid(Music), path);

        auto it = m_resourceCache.find(cacheKey);
        if (it != m_resourceCache.end())
        {
            return std::static_pointer_cast<Music>(it->second);
        }

        std::string fullPath = MakeFullPath(path);

        auto music = std::shared_ptr<Music>(new Music());
        if (!music->LoadFromFile(fullPath))
        {
            std::cerr << "ResourceManager: failed to load music: " << fullPath << "\n";
            return nullptr;
        }

        m_resourceCache[cacheKey] = music;
        return music;
    }
}
