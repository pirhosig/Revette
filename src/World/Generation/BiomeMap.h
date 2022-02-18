#pragma once
#include <array>

#include "../ChunkPos.h"
#include "../../Constants.h"
class NoiseSource2D;



class BiomeMap
{
public:
	BiomeMap(ChunkPos2D noisePos, NoiseSource2D& noiseTemperature, NoiseSource2D& noiseHumidity);
	BiomeMap(const BiomeMap&) = delete;

	std::array<int, CHUNK_AREA> biomeArray;
};