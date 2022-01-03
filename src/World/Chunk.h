#pragma once
#include <array>
#include <unordered_map>

#include "Block.h"

constexpr int CHUNK_SIZE = 32;
constexpr int CHUNK_AREA = CHUNK_SIZE * CHUNK_SIZE;
constexpr int CHUNK_VOLUME = CHUNK_AREA * CHUNK_SIZE;



class Chunk
{
public:

private:
	std::array<uint16_t, CHUNK_VOLUME> blockArray;
};