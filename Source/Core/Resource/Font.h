#pragma once

#include "Core/Math/Region.h"

#include <memory>
#include <string>

namespace KatanaEngine
{
	class ResourceManager;
	class SpriteBatch;

	class Font
	{
	public:
		~Font() = default;

		Font(Font&&) noexcept = default;
		Font& operator=(Font&&) noexcept = default;

		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;

		int GetSize() const { return m_size; }
		int GetLineHeight() const;
		int GetAscent() const;
		int MeasureString(const std::string& text) const;
		// The box the text's glyphs actually cover (their ink) when drawn with its top left at 0,0, in pixels at the
		// font's size: X and Y can be negative (a glyph reaching left of or above the line) and the box can reach
		// past MeasureString's width or the line height (an italic's slant, a descender). Empty for empty text or an
		// invalid font.
		Region MeasureInkBounds(const std::string& text) const;
		bool IsValid() const;

	private:
		friend class ResourceManager;
		friend class SpriteBatch;

		Font();

		struct FontData;
		std::unique_ptr<FontData> m_pData;
		int m_size = 0;
	};
}
