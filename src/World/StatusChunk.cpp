#include "StatusChunk.h"

#include <cassert>


inline int convertIndex(int xOffset, int yOffset, int zOffset)
{
	assert((xOffset >= -1 && xOffset <= 1 &&
			yOffset >= -1 && yOffset <= 1 &&
			yOffset >= -1 && yOffset <= 1),
		"Chunk position is not a neighbour.");
	int ind = (xOffset + 1) * 9 + (yOffset + 1) * 3 + zOffset;
	if (ind < 13) ind--;
	return ind;
}



void StatusChunk::setNeighbourLoadStatus(int xOffset, int yOffset, int zOffset, bool isLoaded)
{
	neighboursLoaded[convertIndex(xOffset, yOffset, zOffset)] = isLoaded;
}



bool StatusChunk::getNeighboursCardinalAreLoaded() const
{
	return (
		neighboursLoaded[convertIndex( 0,  1,  0)] &&
		neighboursLoaded[convertIndex( 0, -1,  0)] &&
		neighboursLoaded[convertIndex( 1,  0,  0)] &&
		neighboursLoaded[convertIndex(-1,  0,  0)] &&
		neighboursLoaded[convertIndex( 0,  0,  1)] &&
		neighboursLoaded[convertIndex( 0,  0, -1)]
		);
}



bool StatusChunk::getNeighboursCubeAreLoaded() const
{
	bool allAreLoaded = true;
	for (int i = 0; i < 26; ++i)
	{
		allAreLoaded = allAreLoaded && neighboursLoaded[i];
	}
	return allAreLoaded;
}