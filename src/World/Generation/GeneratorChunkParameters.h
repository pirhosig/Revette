#pragma once
#include "HeightMap.h"
#include "BiomeMap.h"
#include "GeneratorChunkNoise.h"
class Chunk;



class GeneratorChunkParameters
{
public:
	GeneratorChunkParameters(ChunkPos2D noisePos, GeneratorChunkNoise& noiseParameters) :
		heightMap(noisePos, noiseParameters.noiseHeight),
		biomeMap(noisePos, noiseParameters.noiseTemperature, noiseParameters.noiseRainfall, noiseParameters.noiseBiomeEdge)
	{}
	GeneratorChunkParameters(const GeneratorChunkParameters&) = delete;

private:
	HeightMap heightMap;
	BiomeMap biomeMap;

	friend Chunk;
};
