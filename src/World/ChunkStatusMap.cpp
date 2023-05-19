#include "ChunkStatusMap.h"



bool ChunkStatusMap::chunkExists(const ChunkPos chunkPos) const
{
	return statusMap.contains(chunkPos);
}



bool ChunkStatusMap::getChunkStatusCanMesh(const ChunkPos chunkPos) const
{
	auto chunkStatusIterator = statusMap.find(chunkPos);
	if (chunkStatusIterator == statusMap.end()) return false;
	else return chunkStatusIterator->second.canMesh();
}



bool ChunkStatusMap::getChunkStatusCanPopulate(const ChunkPos chunkPos) const
{
	auto chunkStatusIterator = statusMap.find(chunkPos);
	if (chunkStatusIterator == statusMap.end()) return false;
	else return chunkStatusIterator->second.canPopulate();
}



StatusChunkLoad ChunkStatusMap::getChunkStatusLoad(const ChunkPos chunkPos) const
{
	auto chunkStatusIterator = statusMap.find(chunkPos);
	if (chunkStatusIterator == statusMap.end()) return StatusChunkLoad::NON_EXISTENT;
	else return chunkStatusIterator->second.getLoadStatus();
}



StatusChunkMesh ChunkStatusMap::getChunkStatusMesh(const ChunkPos chunkPos) const
{
	auto chunkStatusIterator = statusMap.find(chunkPos);
	if (chunkStatusIterator == statusMap.end()) return StatusChunkMesh::NON_EXISTENT;
	else return chunkStatusIterator->second.getMeshStatus();
}



void ChunkStatusMap::setChunkStatusLoad(const ChunkPos chunkPos, StatusChunkLoad status)
{
	if (status == StatusChunkLoad::NON_EXISTENT) statusMap.erase(chunkPos);
	else
	{
		bool isNew = !statusMap.contains(chunkPos);
		statusMap[chunkPos].setLoadStatus(status);
		if (isNew)
		{
			// Get neighbour statuses
			for (int i = -1; i <= 1; ++i)
				for (int j = -1; j <= 1; ++j)
					for (int k = -1; k <= 1; ++k)
					{
						if (i == 0 && j == 0 && k == 0) continue;
						statusMap[chunkPos].setNeighbourLoadStatus(
							i, j, k,
							getChunkStatusLoad(ChunkPos(chunkPos.x + i, chunkPos.y + j, chunkPos.z + k))
						);
					}
		}
	}
	
	// Update the neighbouring chunk statuses, if they exist
	for (int i = -1; i <= 1; ++i)
		for (int j = -1; j <= 1; ++j)
			for (int k = -1; k <= 1; ++k)
			{
				if (i == 0 && j == 0 && k == 0) continue;
				ChunkPos pos(chunkPos.x + i, chunkPos.y + j, chunkPos.z + k);
				if (chunkExists(pos)) statusMap.at(pos).setNeighbourLoadStatus(-i, -j, -k, status);
			}
}



void ChunkStatusMap::setChunkStatusMesh(const ChunkPos chunkPos, StatusChunkMesh status)
{
	statusMap.at(chunkPos).setHasMesh(status);
}