#include "pch.h"

#include "ResourceManager.h"
#include "Core/Game.h"

#include "../Platform/Platform.h"
#include "Core/Resource/Texture.h"
#include "Core/Platform/PlatformTexture.h"
#include "Core/Resource/Font.h"
#include "Core/Platform/PlatformFont.h"
#include "Core/Resource/AnimationSet.h"
#include "Core/Resource/Effect.h"

#include <filesystem>
#include "Core/Resource/Sound.h"
#include "Core/Resource/Music.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/path.h>

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
    { 
        // Build the full content path
        std::string basePath = Platform::GetExecutableDirectory() + contentRoot;
        std::cout << "Base path before canonicalization: " << basePath << std::endl;

        // Manual path resolution: resolve .. segments ourselves
        ALLEGRO_PATH* path = al_create_path(basePath.c_str());
        if (path)
        {
            // Get the number of directory components
            int componentCount = al_get_path_num_components(path);

            // Build a new path by processing components and removing .. references
            ALLEGRO_PATH* resolvedPath = al_create_path("");

            for (int i = 0; i < componentCount; ++i)
            {
                const char* component = al_get_path_component(path, i);

                if (strcmp(component, "..") == 0)
                {
                    // Remove the last directory from resolvedPath
                    int resolvedCount = al_get_path_num_components(resolvedPath);
                    if (resolvedCount > 0)
                    {
                        al_remove_path_component(resolvedPath, resolvedCount - 1);
                    }
                }
                else if (strcmp(component, ".") != 0)
                {
                    // Add this component (skip "." as it means current directory)
                    al_append_path_component(resolvedPath, component);
                }
            }

            // Preserve the filename if there was one
            const char* filename = al_get_path_filename(path);
            if (filename && strlen(filename) > 0)
            {
                al_set_path_filename(resolvedPath, filename);
            }

            // Get the drive (important for Windows absolute paths)
            const char* drive = al_get_path_drive(path);
            if (drive && strlen(drive) > 0)
            {
                al_set_path_drive(resolvedPath, drive);
            }

            m_contentRoot = al_path_cstr(resolvedPath, ALLEGRO_NATIVE_PATH_SEP);
            std::cout << "Content root resolved to: " << m_contentRoot << std::endl;

            al_destroy_path(resolvedPath);
            al_destroy_path(path);
        }
        else
        {
            m_contentRoot = basePath; // Fallback
            std::cerr << "WARNING: Failed to create Allegro path object" << std::endl;
        }
    }

    std::string ResourceManager::MakeFullPath(const std::string& path) const
    {
        // Absolute paths (e.g. files a user picked in a tool) bypass the content root.
        if (m_contentRoot.empty() || std::filesystem::path(path).is_absolute()) return path;

        // Build the combined path
        std::string fullPath;
        char lastChar = m_contentRoot.back();
        if (lastChar == '/' || lastChar == '\\')
        {
            fullPath = m_contentRoot + path;
        }
        else
        {
            fullPath = m_contentRoot + "/" + path;
        }

        // Canonicalize the final path to resolve any remaining .. or . segments
        ALLEGRO_PATH* alPath = al_create_path(fullPath.c_str());
        if (alPath)
        {
            if (al_make_path_canonical(alPath))
            {
                fullPath = al_path_cstr(alPath, ALLEGRO_NATIVE_PATH_SEP);
            }
            al_destroy_path(alPath);
        }

        return fullPath;
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

        // Determine shader path based on graphics backend
        std::string shaderPath = path;

        if (m_pGame)
        {
            GraphicsBackend backend = m_pGame->GetGraphicsBackend();

            // Replace "Effects/" with backend-specific folder
            if (path.find("Effects/") == 0)
            {
                std::string filename = path.substr(8); // Remove "Effects/"

                switch (backend)
                {
                    case GraphicsBackend::Direct3D:
                        shaderPath = "Effects/HLSL/" + filename;
                        std::cout << "Loading Direct3D shader: " << shaderPath << std::endl;
                        break;

                    case GraphicsBackend::OpenGL:
                        shaderPath = "Effects/GLSL/" + filename;
                        std::cout << "Loading OpenGL shader: " << shaderPath << std::endl;
                        break;

                    default:
                        LogResourceError("WARNING: No shader support, effect will not load");
                        return nullptr;
                }
            }
        }

        std::string fullPath = MakeFullPath(shaderPath);
        std::cout << "Full shader path resolved to: " << fullPath << std::endl;

        auto effect = std::shared_ptr<Effect>(new Effect());
        if (!effect->LoadFromFile(fullPath))
        {
            LogResourceError("ResourceManager: failed to load effect: " + fullPath);
            // Don't assert - just log and return null for graceful degradation
            LogResourceError("WARNING: Shader failed to load. Continuing without effect.");
            return nullptr;
        }

        std::cout << "Successfully loaded effect: " << shaderPath << std::endl;

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
