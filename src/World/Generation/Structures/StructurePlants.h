#pragma once
#include "../../Block.h"

class Chunk;


namespace Structures
{
	// Boreal Forest
	void placeTreePine(Chunk& chunk, BlockPos base, unsigned long long age, int height);
	void placeTreePineMassive(Chunk& chunk, BlockPos base, unsigned long long age, int height);


	// Rainforest
	void placeTreeRainforestShrub(Chunk& chunk, BlockPos base, unsigned long long age);
	void placeTreeRainforestTall(Chunk& chunk, BlockPos base, unsigned long long age, int height);
}