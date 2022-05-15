#pragma once
#include "../../Block.h"



class StructureBoundingBox
{
public:
	StructureBoundingBox(BlockPos lower, BlockPos upper);

	bool intersects(StructureBoundingBox& other) const;

	BlockPos lowerCorner;
	BlockPos upperCorner;
};