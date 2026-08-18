#pragma once
#include <array>
#include <memory>

#include <FastNoise/FastNoise.h>

#include "../ChunkPos.h"



class NoiseSource2D {
	const FastNoise::SmartNode<> generator;
	const int seed;

public:
	NoiseSource2D(const char* noiseSetting, int _seed);

	NoiseSource2D(NoiseSource2D&&) = delete;
	NoiseSource2D(const NoiseSource2D&) = delete;
	NoiseSource2D operator=(NoiseSource2D&&) = delete;
	NoiseSource2D operator=(const NoiseSource2D&) = delete;

	std::array<float, CHUNK_AREA> genChunkNoise(ChunkPos2D chunkPos) const;
};