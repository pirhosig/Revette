#pragma once
#include <array>



enum class StatusChunkLoad
{
	NON_EXISTENT,
	QUEUED,
	LOADED,
	GENERATED,
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
	bool getNeighboursCardinalHaveStatus(StatusChunkLoad loadStatus) const;
	bool getNeighboursCubeHaveStatus(StatusChunkLoad loadStatus) const;

private:
	StatusChunkLoad loadStatus{ StatusChunkLoad::NON_EXISTENT };
	StatusChunkMesh hasMesh{ StatusChunkMesh::NON_EXISTENT };
	std::array<StatusChunkLoad, 26> neighboursLoaded{};
};