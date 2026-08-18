#pragma once
#include "../../ChunkPos.h"



class GenerationUnitPos2D {
	int32_t x;
	int32_t z;

public:
	GenerationUnitPos2D(int32_t _x, int32_t _z);
	GenerationUnitPos2D(ChunkPos2D pos);

	bool operator==(const GenerationUnitPos2D&) const = default;
	int32_t getX() const;
	int32_t getZ() const;

	GenerationUnitPos2D offsetBy(int32_t xOffset, int32_t zOffset) const;
};
