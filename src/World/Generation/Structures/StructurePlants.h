#pragma once
#include <vector>
#include "../../Block.h"



struct BlockPlace
{
	int xOffset;
	int yOffset;
	int zOffset;
	Block block;
	unsigned long long ageOffset;
};



std::vector<BlockPlace> getStructureTreePine(int height);
