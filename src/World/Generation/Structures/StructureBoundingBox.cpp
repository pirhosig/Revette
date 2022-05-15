#include "StructureBoundingBox.h"



StructureBoundingBox::StructureBoundingBox(BlockPos lower, BlockPos upper) : lowerCorner(lower), upperCorner(upper) {};


// Checks if there is are any blocks that exist in both bounding boxes
bool StructureBoundingBox::intersects(StructureBoundingBox& other) const
{
	return
		(upperCorner.x >= other.lowerCorner.x) && (other.upperCorner.x >= lowerCorner.x) &&
		(upperCorner.y >= other.lowerCorner.y) && (other.upperCorner.y >= lowerCorner.y) &&
		(upperCorner.z >= other.lowerCorner.z) && (other.upperCorner.z >= lowerCorner.z);
}