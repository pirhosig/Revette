#include "StructureBoundingBox.h"



StructureBoundingBox::StructureBoundingBox(BlockPos lower, BlockPos upper) : lowerCorner(lower), upperCorner(upper) {};


// Checks if there is are any blocks that exist in both bounding boxes
bool StructureBoundingBox::intersects(StructureBoundingBox& other) const
{
	return
		(upperCorner.getX() >= other.lowerCorner.getX()) && (other.upperCorner.getX() >= lowerCorner.getX()) &&
		(upperCorner.getY() >= other.lowerCorner.getY()) && (other.upperCorner.getY() >= lowerCorner.getY()) &&
		(upperCorner.getZ() >= other.lowerCorner.getZ()) && (other.upperCorner.getZ() >= lowerCorner.getZ());
}