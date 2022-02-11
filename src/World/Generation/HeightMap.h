#pragma once
#include <array>

#include "../ChunkPos.h"
#include "../../Constants.h"
class NoiseSource2D;



class HeightMap
{
public:
	HeightMap(ChunkPos2D noisePos, NoiseSource2D& noiseSource);
	HeightMap(const HeightMap&) = delete;

	int heightMin;
	int heightMax;
	std::array<int, CHUNK_AREA> heightArray;
};