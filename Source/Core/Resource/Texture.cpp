#include "pch.h"

#include "Texture.h"
#include "Core/Platform/PlatformTexture.h"

namespace KatanaEngine
{
    Texture::~Texture() = default;

    Texture::Texture()
        : m_pData(std::make_unique<TextureData>())
    { }

    void Texture::SetBitmap(ALLEGRO_BITMAP* bitmap)
    {
        if (!m_pData)
        {
            m_pData = std::make_unique<TextureData>();
        }

        if (m_pData->pBitmap && m_pData->pBitmap != bitmap)
        {
            al_destroy_bitmap(m_pData->pBitmap);
        }

        m_pData->pBitmap = bitmap;
    }

    std::shared_ptr<Texture> Texture::CreateFromBitmap(ALLEGRO_BITMAP* bitmap)
    {
        if (!bitmap)
        {
            return nullptr;
        }

        auto texture = std::shared_ptr<Texture>(new Texture());
        texture->SetBitmap(bitmap);
        return texture;
    }

    int Texture::GetWidth() const
    {
        if (m_pData && m_pData->pBitmap)
        {
            return al_get_bitmap_width(m_pData->pBitmap);
        }
        return 0;
    }

    int Texture::GetHeight() const
    {
        if (m_pData && m_pData->pBitmap)
        {
            return al_get_bitmap_height(m_pData->pBitmap);
        }
        return 0;
    }

    bool Texture::IsValid() const
    {
        return m_pData && m_pData->pBitmap != nullptr;
    }

    ALLEGRO_BITMAP* Texture::GetBitmap() const
    {
        if (m_pData)
        {
            return m_pData->pBitmap;
        }
        return nullptr;
    }
}
