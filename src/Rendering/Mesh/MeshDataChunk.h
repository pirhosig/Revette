#pragma once
#include <memory>
#include <vector>

#include "../../World/Chunk.h"



#pragma pack(push, 1)
struct Vertex
{
	uint32_t x: 10;
	uint32_t y: 10;
	uint32_t z: 10;
	uint16_t t;
	uint8_t u;
	uint8_t v;
	uint8_t w;
};
#pragma pack(pop)



class MeshDataChunk
{
public:
	MeshDataChunk(
		ChunkPos chunkPos,
		const std::unique_ptr<Chunk>& chunkCentre,
		const std::unique_ptr<Chunk>& chunkUp,
		const std::unique_ptr<Chunk>& chunkDown,
		const std::unique_ptr<Chunk>& chunkNorth,
		const std::unique_ptr<Chunk>& chunkSouth,
		const std::unique_ptr<Chunk>& chunkEast,
		const std::unique_ptr<Chunk>& chunkWest
	);
	MeshDataChunk(const MeshDataChunk&) = delete;

	ChunkPos position;

	std::vector<Vertex> verticies;
	std::vector<uint32_t> indicies;

	int triangleCount = 0;
};