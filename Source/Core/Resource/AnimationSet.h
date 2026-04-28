#pragma once

#include "TextResource.h"
#include "Animation.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace KatanaEngine
{
    class ResourceManager;

    class AnimationSet : public TextResource
    {
    public:
        AnimationSet();
        ~AnimationSet() override;

        AnimationSet(AnimationSet&&) noexcept;
        AnimationSet& operator=(AnimationSet&&) noexcept;

        AnimationSet(const AnimationSet&) = delete;
        AnimationSet& operator=(const AnimationSet&) = delete;

        bool LoadFromFile(const std::string& path) override;

        // Returns a cloned Animation with its own state
        Animation GetAnimation(const std::string& name) const;

        bool HasAnimation(const std::string& name) const;
        std::vector<std::string> GetAnimationNames() const;

        bool IsValid() const { return !m_animations.empty(); }

    private:
        friend class ResourceManager;

        bool ParseLine(const std::string& line, std::string& key, std::string& value);
        std::vector<int> ParseFrameIndices(const std::string& value);

        std::unordered_map<std::string, Animation> m_animations;
    };
}
