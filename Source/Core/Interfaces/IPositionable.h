#pragma once

namespace KatanaEngine
{
	class Vector2;

	class IPositionable
	{
	public:
		virtual ~IPositionable() {}

		virtual Vector2 GetPosition() const = 0;
	};
}
