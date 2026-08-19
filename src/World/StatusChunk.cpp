#include "StatusChunk.h"

#include <cassert>
#include <cstddef>



namespace {

size_t convertIndex(int xOffset, int yOffset, int zOffset) {
	assert((xOffset >= -1 && xOffset <= 1 &&
			yOffset >= -1 && yOffset <= 1 &&
			yOffset >= -1 && yOffset <= 1) &&
		"Chunk position is not a neighbour.");
	int ind = (xOffset + 1) * 9 + (yOffset + 1) * 3 + (zOffset + 1);
	if (ind > 12) ind--;
	return static_cast<size_t>(ind);
}

}



void StatusChunk::setNeighbourLoadStatus(int xOffset, int yOffset, int zOffset, StatusChunkLoad _loadStatus)
{
	const auto index = convertIndex(xOffset, yOffset, zOffset);
	bool isCardinal = (
		(xOffset == -1 && yOffset ==  0 && zOffset ==  0) ||
		(xOffset ==  1 && yOffset ==  0 && zOffset ==  0) ||
		(xOffset ==  0 && yOffset == -1 && zOffset ==  0) ||
		(xOffset ==  0 && yOffset ==  1 && zOffset ==  0) ||
		(xOffset ==  0 && yOffset ==  0 && zOffset == -1) ||
		(xOffset ==  0 && yOffset ==  0 && zOffset ==  1)
	);
	if (isCardinal) {
		neighbourLoadCountCardinal[static_cast<size_t>(neighboursLoaded[index])]--;
		neighbourLoadCountCardinal[static_cast<size_t>(_loadStatus)]++;
	}
	else {
		neighbourLoadCountCubic[static_cast<size_t>(neighboursLoaded[index])]--;
		neighbourLoadCountCubic[static_cast<size_t>(_loadStatus)]++;
	}
	neighboursLoaded[index] = _loadStatus;
}


bool StatusChunk::canMesh() const {
	return ((hasMesh == StatusChunkMesh::NON_EXISTENT) &&
		(loadStatus == StatusChunkLoad::POPULATED) &&
		(neighbourLoadCountCardinal[static_cast<size_t>(StatusChunkLoad::POPULATED)] == 6)
	);
}



bool StatusChunk::canPopulate() const {
	int count = (neighbourLoadCountCardinal[static_cast<size_t>(StatusChunkLoad::GENERATED)]
		+ neighbourLoadCountCubic[static_cast<size_t>(StatusChunkLoad::GENERATED)]
		+ neighbourLoadCountCardinal[static_cast<size_t>(StatusChunkLoad::QUEUED_POPULATE)]
		+ neighbourLoadCountCubic[static_cast<size_t>(StatusChunkLoad::QUEUED_POPULATE)]
		+ neighbourLoadCountCardinal[static_cast<size_t>(StatusChunkLoad::POPULATED)]
		+ neighbourLoadCountCubic[static_cast<size_t>(StatusChunkLoad::POPULATED)]);
	return ((loadStatus == StatusChunkLoad::GENERATED) && (count == 26));
}