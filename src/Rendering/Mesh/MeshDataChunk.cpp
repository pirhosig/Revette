#include "MeshDataChunk.h"

#include "../../World/World.h"



const uint16_t BLOCK_TEXUTRES[][6] = {
	{0, 0, 0, 0, 0, 0},
	{2, 2, 2, 2, 2, 2},
	{3, 3, 3, 3, 3, 3},
	{4, 4, 4, 4, 4, 4},
	{5, 5, 5, 5, 5, 5},
	{6, 6, 6, 6, 6, 6},
	{7, 7, 7, 7, 7, 7},
	{8, 8, 8, 8, 8, 8},
	{9, 9, 9, 9, 9, 9}
};



// Offsets for every vertex to draw a cube
const int FACE_TABLE[6][4][3] = {
	// Up
	{{ 0, 1, 0 }, { 1, 1, 0 }, { 1, 1, 1 }, { 0, 1, 1 }},
	// Down
	{{ 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 }},
	// North
	{{ 1, 1, 1 }, { 1, 1, 0 }, { 1, 0, 0 }, { 1, 0, 1 }},
	// South
	{{ 0, 1, 1 }, { 0, 1, 0 }, { 0, 0, 0 }, { 0, 0, 1 }},
	// East
	{{ 0, 1, 1 }, { 1, 1, 1 }, { 1, 0, 1 }, { 0, 0, 1 }},
	// West
	{{ 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 }, { 0, 0, 0 }}
};

const uint8_t TEXTURE_COORDINATES[4][2] = {
	{   0,   0 },
	{ 255,   0 },
	{ 255, 255 },
	{   0, 255 }
};



const uint8_t LIGHT[6] = {
	255,
	229,
	240,
	240,
	220,
	220
};



MeshDataChunk::MeshDataChunk(
	ChunkPos chunkPos,
	const std::unique_ptr<Chunk>& chunkCentre,
	const std::unique_ptr<Chunk>& chunkUp,
	const std::unique_ptr<Chunk>& chunkDown,
	const std::unique_ptr<Chunk>& chunkNorth,
	const std::unique_ptr<Chunk>& chunkSouth,
	const std::unique_ptr<Chunk>& chunkEast,
	const std::unique_ptr<Chunk>& chunkWest
) : position(chunkPos)
{
	triangleCount = 0;
	int indexCounter = 0;

	// Skip loop if chunk is empty
	if (chunkCentre->isEmpty()) return;

	// Loop and check for each block whether it is solid, and so whether it needs to be added
	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int j = 0; j < CHUNK_SIZE; ++j)
		{
			for (int k = 0; k < CHUNK_SIZE; ++k)
			{
				ChunkLocalBlockPos localPos(i, j, k);
				BlockPos worldPos = localPos.asBlockPos(chunkPos);

				Block block = chunkCentre->getBlock(worldPos);
				// Skip if air block
				if (block.blockType == 0) continue;

				// Loop through all block faces
				for (int l = 0; l < 6; ++l)
				{
					AxisDirection neighborDirection = static_cast<AxisDirection>(l);
					BlockPos neighborBlock = worldPos.direction(neighborDirection);
					ChunkLocalBlockPos neighborLocal(neighborBlock);
					Block neighbor;
					if (ChunkPos(neighborBlock) == position) neighbor = chunkCentre->getBlock(neighborLocal);
					else
					{
						switch (neighborDirection)
						{
						case AxisDirection::Up:
							neighbor = chunkUp->getBlock(neighborLocal);
							break;
						case AxisDirection::Down:
							neighbor = chunkDown->getBlock(neighborLocal);
							break;
						case AxisDirection::North:
							neighbor = chunkNorth->getBlock(neighborLocal);
							break;
						case AxisDirection::South:
							neighbor = chunkSouth->getBlock(neighborLocal);
							break;
						case AxisDirection::East:
							neighbor = chunkEast->getBlock(neighborLocal);
							break;
						case AxisDirection::West:
							neighbor = chunkWest->getBlock(neighborLocal);
							break;
						default:
							break;
						}
					}

					// Only add face if the adjacent block is transparent
					if (neighbor.blockType != 0) continue;

					// Set winding order, to enable GPU based baceface culling
					if (l % 2)
					{
						indicies.push_back(indexCounter);
						indicies.push_back(indexCounter + 1);
						indicies.push_back(indexCounter + 2);
						indicies.push_back(indexCounter + 2);
						indicies.push_back(indexCounter + 3);
						indicies.push_back(indexCounter);
					}
					else
					{
						indicies.push_back(indexCounter);
						indicies.push_back(indexCounter + 2);
						indicies.push_back(indexCounter + 1);
						indicies.push_back(indexCounter + 2);
						indicies.push_back(indexCounter);
						indicies.push_back(indexCounter + 3);
					}
					triangleCount += 2;
					indexCounter += 4;

					// Loop through each vertex of the face
					for (int v = 0; v < 4; ++v)
					{
						Vertex vertex{
							static_cast<uint16_t>(localPos.x + FACE_TABLE[l][v][0]),
							static_cast<uint16_t>(localPos.y + FACE_TABLE[l][v][1]),
							static_cast<uint16_t>(localPos.z + FACE_TABLE[l][v][2]),
							BLOCK_TEXUTRES[block.blockType - 1][l],
							TEXTURE_COORDINATES[v][0],
							TEXTURE_COORDINATES[v][1],
							LIGHT[l]
						};
						verticies.push_back(vertex);
					}
				}
			}
		}
	}
}