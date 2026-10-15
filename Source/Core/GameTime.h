#pragma once

namespace KatanaEngine
{
	struct GameTime
	{
		double TotalTime;
		double ElapsedTime;
		bool IsRunningSlowly;

		GameTime()
			: TotalTime(0.0)
			, ElapsedTime(0.0)
			, IsRunningSlowly(false)
		{
		}

		GameTime(double totalTime, double elapsedTime, bool isRunningSlowly = false)
			: TotalTime(totalTime)
			, ElapsedTime(elapsedTime)
			, IsRunningSlowly(isRunningSlowly)
		{
		}
	};
}
