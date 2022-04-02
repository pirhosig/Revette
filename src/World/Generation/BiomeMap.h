#pragma once
#include <array>

#include "../ChunkPos.h"
#include "../../Constants.h"
class NoiseSource2D;


enum class BIOME
{
	DESERT,
	FOREST,
	TUNDRA
};



class BiomeMap
{
public:
	BiomeMap(ChunkPos2D noisePos, NoiseSource2D& noiseTemperature, NoiseSource2D& noiseHumidity, NoiseSource2D& noiseBiomeEdge);
	BiomeMap(const BiomeMap&) = delete;

	std::array<BIOME, CHUNK_AREA> biomeArray;
};