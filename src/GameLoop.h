#pragma once
#include <atomic>



class GameLoop
{
public:

	void runLoop(std::atomic<bool>& gameShouldClose);
private:
};