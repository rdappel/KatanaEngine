#pragma once

#include <string>

struct ALLEGRO_BITMAP;

namespace KatanaEngine
{
    class Texture;

    // A CPU-side (memory) bitmap for building and saving images, e.g. sprite sheets
    // assembled from RenderTarget frames. It is not limited by the GPU's maximum
    // texture size. It can't be drawn with SpriteBatch directly: CopyTo a RenderTarget first.
    class Image
    {
    public:
        Image() = default;
        ~Image();

        Image(const Image&) = delete;
        Image& operator=(const Image&) = delete;

        // Creates a fully transparent image, replacing any previous contents.
        bool Create(int width, int height);

        // Copies the texture's pixels unchanged (no blending) with its top-left at (x, y).
        void CopyFrom(const Texture& texture, int x, int y);

        // Copies the area of this image at (sourceX, sourceY), the size of the texture,
        // into the texture (e.g. a RenderTarget), replacing its pixels. Parts outside
        // this image become transparent.
        void CopyTo(Texture& texture, int sourceX, int sourceY) const;

        // Saves to a file whose format comes from the extension (.png keeps alpha).
        // Pixels are stored premultiplied, as everything is drawn; the file is written
        // with straight alpha, which is what image loaders (including Allegro) expect.
        bool Save(const std::string& path) const;

        int GetWidth() const;
        int GetHeight() const;
        bool IsValid() const { return m_pBitmap != nullptr; }

    private:
        void Destroy();

        ALLEGRO_BITMAP* m_pBitmap = nullptr;
    };
}
