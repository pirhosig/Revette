#pragma once
#include <array>



enum class StatusChunkLoad
{
	NON_EXISTENT,
	QUEUED,
	LOADED,
	GENERATED,
};



class StatusChunk
{
public:
	StatusChunkLoad getLoadStatus() const { return loadStatus; }
	void setLoadStatus(StatusChunkLoad _loadStatus) { loadStatus = _loadStatus; }
	bool getHasMesh() const { return hasMesh; }
	void setHasMesh(bool _hasMesh) { hasMesh = _hasMesh; }

	void setNeighbourLoadStatus(int xOffset, int yOffset, int zOffset, bool isLoaded);
	bool getNeighboursCardinalAreLoaded() const;
	bool getNeighboursCubeAreLoaded() const;

private:
	StatusChunkLoad loadStatus{ StatusChunkLoad::NON_EXISTENT };
	bool hasMesh{ false };
	std::array<bool, 26> neighboursLoaded{};
};