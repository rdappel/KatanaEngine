#include "pch.h"

#include "AnimationSet.h"

#include <fstream>
#include <sstream>
#include <algorithm>

namespace KatanaEngine
{
    AnimationSet::AnimationSet() = default;
    AnimationSet::~AnimationSet() = default;

    AnimationSet::AnimationSet(AnimationSet&&) noexcept = default;
    AnimationSet& AnimationSet::operator=(AnimationSet&&) noexcept = default;

    bool AnimationSet::LoadFromFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
            return false;

        m_animations.clear();

        std::string currentSection;
        Animation currentAnim;

        std::string line;
        while (std::getline(file, line))
        {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';')
                continue;

            // Check for section header [SectionName]
            if (line[0] == '[' && line.back() == ']')
            {
                // Save previous animation if exists
                if (!currentSection.empty() && currentAnim.IsValid())
                {
                    m_animations[currentSection] = currentAnim;
                }

                // Start new section
                currentSection = line.substr(1, line.size() - 2);
                currentAnim = Animation();
                continue;
            }

            // Parse key=value
            std::string key, value;
            if (ParseLine(line, key, value) && !currentSection.empty())
            {
                if (key == "FrameWidth")
                {
                    currentAnim.m_frameWidth = std::stoi(value);
                }
                else if (key == "FrameHeight")
                {
                    currentAnim.m_frameHeight = std::stoi(value);
                }
                else if (key == "Frames")
                {
                    currentAnim.m_frameIndices = ParseFrameIndices(value);
                }
                else if (key == "FrameDuration")
                {
                    currentAnim.m_frameDuration = std::stof(value);
                }
                else if (key == "Loop")
                {
                    std::string lower = value;
                    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                    currentAnim.m_loop = (lower == "true" || lower == "1" || lower == "yes");
                }
                else if (key == "FramesPerRow")
                {
                    currentAnim.m_framesPerRow = std::stoi(value);
                }
            }
        }

        // Save last animation
        if (!currentSection.empty() && currentAnim.IsValid())
        {
            m_animations[currentSection] = currentAnim;
        }

        return !m_animations.empty();
    }

    Animation AnimationSet::GetAnimation(const std::string& name) const
    {
        auto it = m_animations.find(name);
        if (it != m_animations.end())
        {
            return it->second.Clone();
        }
        return Animation();
    }

    bool AnimationSet::HasAnimation(const std::string& name) const
    {
        return m_animations.find(name) != m_animations.end();
    }

    std::vector<std::string> AnimationSet::GetAnimationNames() const
    {
        std::vector<std::string> names;
        names.reserve(m_animations.size());
        for (const auto& pair : m_animations)
        {
            names.push_back(pair.first);
        }
        return names;
    }

    bool AnimationSet::ParseLine(const std::string& line, std::string& key, std::string& value)
    {
        size_t pos = line.find('=');
        if (pos == std::string::npos)
            return false;

        key = line.substr(0, pos);
        value = line.substr(pos + 1);

        // Trim whitespace from key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        return !key.empty();
    }

    std::vector<int> AnimationSet::ParseFrameIndices(const std::string& value)
    {
        std::vector<int> indices;
        std::stringstream ss(value);
        std::string token;

        while (std::getline(ss, token, ','))
        {
            // Trim whitespace
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);

            if (!token.empty())
            {
                // Check for range (e.g., "0-5")
                size_t dashPos = token.find('-');
                if (dashPos != std::string::npos && dashPos > 0)
                {
                    int start = std::stoi(token.substr(0, dashPos));
                    int end = std::stoi(token.substr(dashPos + 1));
                    for (int i = start; i <= end; ++i)
                    {
                        indices.push_back(i);
                    }
                }
                else
                {
                    indices.push_back(std::stoi(token));
                }
            }
        }

        return indices;
    }
}
