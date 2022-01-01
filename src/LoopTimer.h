#pragma once
#include <vector>



class LoopTimer
{
public:
	LoopTimer(int timeNumber);
	void addTime(long long time);

	double averageLoopTime;

private:
	const int timeCount;
	std::vector<long long> loopTimes;
	int updateIndex;
	long long total;
};