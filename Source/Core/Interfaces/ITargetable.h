#pragma once

#include "IPositionable.h"

namespace KatanaEngine
{
	class ITargetable : public IPositionable
	{
	public:
		virtual ~ITargetable() { }
	};
}
