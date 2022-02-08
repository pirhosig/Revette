#include "StatusChunk.h"

#include <cassert>


inline int convertIndex(int xOffset, int yOffset, int zOffset)
{
	assert((xOffset >= -1 && xOffset <= 1 &&
			yOffset >= -1 && yOffset <= 1 &&
			yOffset >= -1 && yOffset <= 1) &&
		"Chunk position is not a neighbour.");
	int ind = (xOffset + 1) * 9 + (yOffset + 1) * 3 + (zOffset + 1);
	if (ind > 12) ind--;
	return ind;
}



void StatusChunk::setNeighbourLoadStatus(int xOffset, int yOffset, int zOffset, StatusChunkLoad loadStatus)
{
	neighboursLoaded[convertIndex(xOffset, yOffset, zOffset)] = loadStatus;
}


bool StatusChunk::getNeighboursCardinalHaveStatus(StatusChunkLoad loadStatus) const
{
	return (
		(neighboursLoaded[convertIndex( 0,  1,  0)] == loadStatus) &&
		(neighboursLoaded[convertIndex( 0, -1,  0)] == loadStatus) &&
		(neighboursLoaded[convertIndex( 1,  0,  0)] == loadStatus) &&
		(neighboursLoaded[convertIndex(-1,  0,  0)] == loadStatus) &&
		(neighboursLoaded[convertIndex( 0,  0,  1)] == loadStatus) &&
		(neighboursLoaded[convertIndex( 0,  0, -1)] == loadStatus)
	);
}



bool StatusChunk::getNeighboursCubeHaveStatus(StatusChunkLoad loadStatus) const
{
	bool allAreLoaded = true;
	for (int i = 0; i < 26; ++i)
	{
		allAreLoaded = allAreLoaded && (neighboursLoaded[i] == loadStatus);
	}
	return allAreLoaded;
}