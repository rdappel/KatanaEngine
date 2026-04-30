#include "pch.h"

#include "ResourceManager.h"

#include "../Platform/Platform.h"
#include "Core/Resource/Texture.h"
#include "Core/Platform/PlatformTexture.h"
#include "Core/Resource/Font.h"
#include "Core/Platform/PlatformFont.h"
#include "Core/Resource/AnimationSet.h"
#include "Core/Resource/Effect.h"

namespace KatanaEngine
{
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
            return nullptr;
        }

        m_resourceCache[cacheKey] = effect;
        return effect;
    }
}
