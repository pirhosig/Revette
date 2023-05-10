#pragma once
#include <random>
#include "../ChunkPos.h"



class ChunkPRNG
{
public:
	ChunkPRNG(ChunkPos pos);
	double next();
	int scaledInt(double a, double b);
	uint16_t raw();

private:
	std::minstd_rand lcg;
};
