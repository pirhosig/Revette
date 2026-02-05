#pragma once
#include <array>
#include <memory>

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

private:
	const FastNoise::SmartNode<> generator;
	const int seed;
};