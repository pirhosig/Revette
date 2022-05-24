#pragma once
#include <array>



enum class StatusChunkLoad
{
	NON_EXISTENT,
	QUEUED_UNLOAD,
	QUEUED_LOAD,
	LOADED,
	GENERATED,
	QUEUED_POPULATE,
	POPULATED
};



enum class StatusChunkMesh
{
	NON_EXISTENT,
	QUEUED,
	MESHED
};



class StatusChunk
{
public:
	StatusChunkLoad getLoadStatus() const { return loadStatus; }
	void setLoadStatus(StatusChunkLoad _loadStatus) { loadStatus = _loadStatus; }
	StatusChunkMesh getMeshStatus() const { return hasMesh; }
	void setHasMesh(StatusChunkMesh _hasMesh) { hasMesh = _hasMesh; }

	void setNeighbourLoadStatus(int xOffset, int yOffset, int zOffset, StatusChunkLoad loadStatus);
	bool canMesh() const;
	bool canPopulate() const;

private:
	StatusChunkLoad loadStatus{ StatusChunkLoad::NON_EXISTENT };
	StatusChunkMesh hasMesh{ StatusChunkMesh::NON_EXISTENT };
	std::array<StatusChunkLoad, 26> neighboursLoaded{};
	std::array<int, 7> neighbourLoadCountCardinal{ 6 };
	std::array<int, 7> neighbourLoadCountCubic{ 20 };
};