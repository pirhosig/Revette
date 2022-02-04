#include "MeshDataChunk.h"

#include "../../World/World.h"
#include <iostream>


MeshDataChunk::MeshDataChunk(const World& world, ChunkPos chunkPos) : position(chunkPos)
{
	// Offsets for every vertex to draw a cube
	const int FACE_TABLE[6][4][3] = {
		// Up
		{{ 0, 1, 0 }, { 1, 1, 0 }, { 1, 1, 1 }, {0, 1, 1}},
		// Down
		{{ 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, {0, 0, 1}},
		// North
		{{ 1, 1, 1 }, { 1, 1, 0 }, { 1, 0, 0 }, {1, 0, 1}},
		// South
		{{ 0, 1, 1 }, { 0, 1, 0 }, { 0, 0, 0 }, {0, 0, 1}},
		// East
		{{ 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 }, {0, 1, 1}},
		// West
		{{ 0, 0, 0 }, { 1, 0, 0 }, { 1, 1, 0 }, {0, 1, 0}}
	};

	const int TEXTURE_COORDINATES[4][2] = {
		{   0,   0 },
		{ 255,   0 },
		{ 255, 255 },
		{   0, 255 }
	};

	triangleCount = 0;
	int indexCounter = 0;

	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int j = 0; j < CHUNK_SIZE; ++j)
		{
			for (int k = 0; k < CHUNK_SIZE; ++k)
			{
				ChunkLocalBlockPos localPos(i, j, k);
				BlockPos worldPos = localPos.asBlockPos(chunkPos);

				Block block = world.getBlock(worldPos);
				// Skip if air block
				if (block.blockType == 0) continue;

				// Loop through all block faces
				for (int l = 0; l < 6; ++l)
				{
					AxisDirection neighborDirection = static_cast<AxisDirection>(l);
					BlockPos neighborBlock = worldPos.direction(neighborDirection);
					Block neighbor = Block(0);
					if (ChunkPos(neighborBlock) == position) neighbor = world.getBlock(neighborBlock);

					// Only add face if the adjacent block is transparent
					if (neighbor.blockType != 0) continue;

					triangleCount += 2;

					indicies.push_back(indexCounter);
					indicies.push_back(indexCounter + 1);
					indicies.push_back(indexCounter + 2);
					indicies.push_back(indexCounter + 2);
					indicies.push_back(indexCounter + 3);
					indicies.push_back(indexCounter);
					indexCounter += 4;

					// Loop through each vertex of the face
					for (int v = 0; v < 4; ++v)
					{
						Vertex vertex{
							static_cast<uint16_t>(localPos.x + FACE_TABLE[l][v][0]),
							static_cast<uint16_t>(localPos.y + FACE_TABLE[l][v][1]),
							static_cast<uint16_t>(localPos.z + FACE_TABLE[l][v][2]),
							static_cast<uint8_t>(block.blockType - 1),
							static_cast<uint8_t>(TEXTURE_COORDINATES[v][0]),
							static_cast<uint8_t>(TEXTURE_COORDINATES[v][1])
						};
						verticies.push_back(vertex);
					}
				}
			}
		}
	}
}