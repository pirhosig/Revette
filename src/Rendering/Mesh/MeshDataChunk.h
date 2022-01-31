#pragma once
#include <vector>

#include "../../World/ChunkPos.h"
class World;



#pragma pack(push, 1)
struct Vertex
{
	uint16_t x;
	uint16_t y;
	uint16_t z;
	uint16_t t;
	uint8_t u;
	uint8_t v;
};
#pragma pack(pop)



class MeshDataChunk
{
public:
	MeshDataChunk(const World& world, ChunkPos chunkPos);
	MeshDataChunk(const MeshDataChunk&) = delete;

	ChunkPos position;

	std::vector<Vertex> verticies;
	std::vector<uint32_t> indicies;

	int triangleCount = 0;
};