#pragma once
#include <array>
#include <FastNoise/FastNoise.h>

#include "../ChunkPos.h"



class NoiseSource2D
{
public:
	NoiseSource2D(const char* noiseSetting, int _seed) :
		generator(FastNoise::NewFromEncodedNodeTree(noiseSetting)),
		seed{ _seed }
	{}
	NoiseSource2D(const NoiseSource2D&) = delete;
	std::array<float, CHUNK_AREA> genChunkNoise(ChunkPos2D chunkPos) const;
	std::unique_ptr<float[]> genChunkNoiseLOD(ChunkPos2D chunkPos, unsigned char level) const;

private:
	const FastNoise::SmartNode<> generator;
	const int seed;
};