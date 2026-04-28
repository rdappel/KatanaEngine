#pragma once

#include <string>
#include <memory>

namespace KatanaEngine
{
    class TextResource
    {
    public:
        virtual ~TextResource() = default;

        virtual bool LoadFromFile(const std::string& path) = 0;

    protected:
        TextResource() = default;

        // Prevent copying - use Clone() for resources that support it
        TextResource(const TextResource&) = default;
        TextResource& operator=(const TextResource&) = default;
    };
}
