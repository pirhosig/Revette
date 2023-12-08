#pragma once
#include <memory>
#include <vector>

#include "../../World/ChunkPos.h"
class Chunk;



#pragma pack(push, 4)
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
	MeshDataChunk(const Chunk* chunkCentre, const std::array<Chunk*, 6> neighbours);
	MeshDataChunk(const MeshDataChunk&) = delete;

	ChunkPos position;

	std::vector<Vertex> verticies;
	std::vector<uint32_t> indicies;

	unsigned triangleCountOpaque;
	unsigned triangleCountTransparent;
};