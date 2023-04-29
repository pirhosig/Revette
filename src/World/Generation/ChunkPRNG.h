#pragma once
#include <random>
#include "../ChunkPos.h"



class ChunkPRNG
{
public:
	ChunkPRNG(ChunkPos pos);
	double getNext();
	int getScaledInt(double a, double b);

private:
	std::minstd_rand lcg;
};
