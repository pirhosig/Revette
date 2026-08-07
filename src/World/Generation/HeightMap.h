#pragma once
#include <array>

#include "Core/RevetteCore.h"
#include "../ChunkPos.h"



class HeightMap
{
public:
	std::array<i16, CHUNK_AREA> heightArray;
	i16 heightMin;
	i16 heightMax;

	HeightMap(ChunkPos2D noisePos, class NoiseSource2D& noiseSource);
	HeightMap(const HeightMap&) = delete;
};