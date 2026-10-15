#pragma once

#include "Texture.h"
#include "Core/Graphics/Color.h"

namespace KatanaEngine
{
	class RenderTarget : public Texture
	{
	public:
		RenderTarget() = default;
		~RenderTarget();

		RenderTarget(RenderTarget&&) noexcept = default;
		RenderTarget& operator=(RenderTarget&&) noexcept = default;

		RenderTarget(const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;

		bool Create(int width, int height);
		void Begin();
		void End();
		void Clear(const Color& color);

		bool IsActive() const { return m_isActive; }

	private:
		ALLEGRO_BITMAP* m_previousTarget = nullptr;
		bool m_isActive = false;
	};
}
