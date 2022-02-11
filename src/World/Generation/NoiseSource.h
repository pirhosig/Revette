#pragma once
#include <array>
#include <FastNoise/FastNoise.h>

#include "../ChunkPos.h"



class NoiseSource2D
{
public:
	NoiseSource2D(const char* noiseSetting, float _frequency, int _seed) :
		generator(FastNoise::NewFromEncodedNodeTree(noiseSetting)),
		frequency{ _frequency },
		seed{ _seed }
	{}
	NoiseSource2D(const NoiseSource2D&) = delete;
	std::array<float, CHUNK_AREA> GenChunkNoise(ChunkPos2D chunkPos) const;

private:
	const FastNoise::SmartNode<> generator;
	const float frequency;
	const int seed;
};