#pragma once
#include <array>

#include "../ChunkPos.h"
#include "../../Constants.h"
class NoiseSource2D;



class HeightMap
{
public:
	std::array<short, CHUNK_AREA> heightArray;
	short heightMin;
	short heightMax;

	HeightMap(ChunkPos2D noisePos, NoiseSource2D& noiseSource);
	HeightMap(const HeightMap&) = delete;
};