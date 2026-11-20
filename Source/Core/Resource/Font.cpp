#include "pch.h"
#include "Font.h"
#include "Core/Platform/PlatformFont.h"

namespace KatanaEngine
{
	Font::Font()
		: m_pData(std::make_unique<FontData>())
		, m_size(0)
	{
	}

	int Font::GetLineHeight() const
	{
		if (m_pData && m_pData->pFont) return al_get_font_line_height(m_pData->pFont);
		return 0;
	}

	int Font::GetAscent() const
	{
		if (m_pData && m_pData->pFont) return al_get_font_ascent(m_pData->pFont);
		return 0;
	}

	int Font::MeasureString(const std::string& text) const
	{
		if (m_pData && m_pData->pFont) return al_get_text_width(m_pData->pFont, text.c_str());
		return 0;
	}

	Region Font::MeasureInkBounds(const std::string& text) const
	{
		if (!m_pData || !m_pData->pFont || text.empty()) return Region();

		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
		al_get_text_dimensions(m_pData->pFont, text.c_str(), &x, &y, &width, &height);
		return Region(x, y, width, height);
	}

	bool Font::IsValid() const
	{
		return m_pData && m_pData->pFont != nullptr;
	}
}
