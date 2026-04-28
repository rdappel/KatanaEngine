#pragma once

#include "IPositionable.h"

namespace KatanaEngine
{
	class ICameraTarget : public IPositionable
	{
	public:
		virtual ~ICameraTarget() { }
	};
}
