#include "LoopTimer.h"



LoopTimer::LoopTimer(int timeNumber) : timeCount(timeNumber), loopTimes(timeNumber, 0)
{
	total = 0;
	updateIndex = 0;
	averageLoopTime = 0.0;
}



void LoopTimer::addTime(long long time)
{
	total -= loopTimes[updateIndex];
	total += time;
	loopTimes[updateIndex] = time;
	updateIndex = (updateIndex + 1) % timeCount;

	averageLoopTime = total / static_cast<double>(timeCount);
}