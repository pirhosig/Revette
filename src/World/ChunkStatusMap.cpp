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
	auto it = statusMap.find(chunkPos);
	bool exists = (it != statusMap.end());
	if (status == StatusChunkLoad::NON_EXISTENT)
	{
		if (exists) statusMap.erase(it);
	}
	else if (exists) statusMap.at(chunkPos).setLoadStatus(status);
	else
	{
		statusMap[chunkPos].setLoadStatus(status);
		for (int i = -1; i < 2; ++i)
		{
			for (int j = -1; j < 2; ++j)
			{
				for (int k = -1; k < 2; ++k)
				{
					if (i == 0 && j == 0 && k == 0) continue;
					ChunkPos pos(chunkPos.x + i, chunkPos.y + j, chunkPos.z + k);
					statusMap[chunkPos].setNeighbourLoadStatus(i, j, k, getChunkStatusLoad(pos));
				}
			}
		}
	}
	
	// Update the neighbouring chunk statuses, if they exist
	for (int i = -1; i < 2; ++i)
	{
		for (int j = -1; j < 2; ++j)
		{
			for (int k = -1; k < 2; ++k)
			{
				if (i == 0 && j == 0 && k == 0) continue;
				ChunkPos pos(chunkPos.x + i, chunkPos.y + j, chunkPos.z + k);
				if (chunkExists(pos)) statusMap[pos].setNeighbourLoadStatus(-i, -j, -k, status);
			}
		}
	}
}



void ChunkStatusMap::setChunkStatusMesh(const ChunkPos chunkPos, StatusChunkMesh status)
{
	statusMap.at(chunkPos).setHasMesh(status);
}