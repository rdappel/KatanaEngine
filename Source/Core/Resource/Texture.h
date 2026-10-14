#pragma once

#include <memory>
#include <string>

struct ALLEGRO_BITMAP;

namespace KatanaEngine
{
    class ResourceManager;
    class RenderTarget;

    class Texture
    {
    public:
		~Texture();

        Texture(Texture&&) noexcept = default;
        Texture& operator=(Texture&&) noexcept = default;

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        int GetWidth() const;
        int GetHeight() const;
        bool IsValid() const;
        ALLEGRO_BITMAP* GetBitmap() const;
        static std::shared_ptr<Texture> CreateFromBitmap(ALLEGRO_BITMAP* bitmap);

    protected:
        friend class ResourceManager;
        friend class RenderTarget;

        Texture();
        void SetBitmap(ALLEGRO_BITMAP* bitmap);

    private:
        struct TextureData;
        std::unique_ptr<TextureData> m_pData;
    };
}
