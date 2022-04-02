#pragma once
#include "HeightMap.h"
#include "BiomeMap.h"
class Chunk;



class GeneratorChunkParameters
{
public:
	GeneratorChunkParameters(ChunkPos2D noisePos, NoiseSource2D& noiseHeight, NoiseSource2D& noiseTemperature, NoiseSource2D& noiseHumidity) :
		heightMap(noisePos, noiseHeight),
		biomeMap(noisePos, noiseTemperature, noiseHumidity)
	{}
	GeneratorChunkParameters(const GeneratorChunkParameters&) = delete;

private:
	HeightMap heightMap;
	BiomeMap biomeMap;

	friend Chunk;
};
