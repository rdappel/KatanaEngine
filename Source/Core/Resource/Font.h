#pragma once

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
