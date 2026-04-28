#pragma once

#include "Core/Math/Rectangle.h"

namespace KatanaEngine
{
	class IBounded
	{
	public:
		virtual ~IBounded() { }

		virtual Rectangle GetBounds() const = 0;
	};
}
