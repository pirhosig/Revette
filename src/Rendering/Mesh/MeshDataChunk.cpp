#include "MeshDataChunk.h"

#include "../../World/World.h"



const uint16_t BLOCK_TEXTURES[][6] = {
	{0,  0,  0,  0,  0,  0 },
	{0,  0,  0,  0,  0,  0 },
	{2,  2,  2,  2,  2,  2 },
	{3,  3,  3,  3,  3,  3 },
	{4,  4,  4,  4,  4,  4 },
	{5,  5,  5,  5,  5,  5 },
	{6,  6,  6,  6,  6,  6 },
	{7,  7,  7,  7,  7,  7 },
	{8,  8,  8,  8,  8,  8 },
	{9,  9,  9,  9,  9,  9 },
	{10, 10, 10, 10, 10, 10},
	{11, 11, 11, 11, 11, 11},
	{12, 12, 12, 12, 12, 12},
	{13, 13, 13, 13, 13, 13},
	{14, 14, 14, 14, 14, 14},
	{15, 15, 15, 15, 15, 15},
	{16, 16, 16, 16, 16, 16},
	{17, 17, 17, 17, 17, 17},
	{18, 18, 18, 18, 18, 18},
};



bool IS_SOLID[] = {
	false,
	true,
	true,
	true,
	false,
	true,
	true,
	true,
	true,
	true,
	false,
	true,
	false,
	true,
	false,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true
};



int MESH_TYPE[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};



bool IS_ROTATEABLE[] = {
	false,
	true,
	true,
	false,
	true,
	true,
	true,
	true,
	true,
	false,
	false,
	true,
	true,
	false,
	false,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true,
	true
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



const uint8_t LIGHT[6] = { 255, 229, 240, 240, 220, 220 };



inline uint32_t basicHash(uint32_t x)
{
	x ^= x >> 16;
	x *= 0x7feb352dU;
	x ^= x >> 15;
	x *= 0x846ca68bU;
	x ^= x >> 16;
	return x;
}



uint32_t getPositionHash(BlockPos pos, uint32_t seedHash)
{
	uint32_t hY = basicHash(static_cast<uint32_t>(pos.y) + 0xe1);
	uint32_t hZ = basicHash(static_cast<uint32_t>(pos.z) + 0xac83);
	return seedHash ^ basicHash(static_cast<uint32_t>(pos.x)) ^ hY ^ hZ;
}



inline int flattenIndex(const ChunkLocalBlockPos blockPos)
{
	// assert(blockPositionIsInside(blockPos.x, blockPos.y, blockPos.z) && "Block outside chunk.");
	return blockPos.x * CHUNK_AREA + blockPos.y * CHUNK_SIZE + blockPos.z;
}



inline void addFaceEven(std::vector<uint32_t>& indicies, int& indexCounter, int& triangleCount)
{
	indicies.push_back(indexCounter);
	indicies.push_back(indexCounter + 2);
	indicies.push_back(indexCounter + 1);
	indicies.push_back(indexCounter + 2);
	indicies.push_back(indexCounter);
	indicies.push_back(indexCounter + 3);
	indexCounter += 4;
	triangleCount += 2;
}



inline void addFaceOdd(std::vector<uint32_t>& indicies, int& indexCounter, int& triangleCount)
{
	indicies.push_back(indexCounter);
	indicies.push_back(indexCounter + 1);
	indicies.push_back(indexCounter + 2);
	indicies.push_back(indexCounter + 2);
	indicies.push_back(indexCounter + 3);
	indicies.push_back(indexCounter);
	indexCounter += 4;
	triangleCount += 2;
}



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

	// Cache transparency
	auto _transparency = chunkCentre->blockContainer.getSolid();

	// Loop and check for each block whether it is solid, and so whether it needs to be added
	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		for (int j = 0; j < CHUNK_SIZE; ++j)
		{
			for (int k = 0; k < CHUNK_SIZE; ++k)
			{
				Block block = chunkCentre->getBlock(ChunkLocalBlockPos(i, j, k));
				// Skip if air block
				if (block.blockType == 0) continue;

				// Solid cube, the most basic and common mesh type
				if (MESH_TYPE[block.blockType] == 0)
				{
					// Add each face if the neighbouring block is not solid
					
					int _index = flattenIndex(ChunkLocalBlockPos(i, j, k));
					int rotationOffset = 0;
					if (IS_ROTATEABLE[block.blockType])
						rotationOffset = static_cast<int>(getPositionHash(
							ChunkLocalBlockPos(i, j, k).asBlockPos(position),
							basicHash(1)) % 4
						);

					// Top face
					if (!(j != CHUNK_SIZE - 1 ?
						_transparency[_index + CHUNK_SIZE] : 
						IS_SOLID[chunkUp->getBlock(ChunkLocalBlockPos(i, 0, k)).blockType]))
					{
						addFaceEven(indicies, indexCounter, triangleCount);
						for (int v = 0; v < 4; ++v)
						{
							verticies.push_back({
								static_cast<uint16_t>(i + FACE_TABLE[0][v][0]) * 16u,
								static_cast<uint16_t>(j + FACE_TABLE[0][v][1]) * 16u,
								static_cast<uint16_t>(k + FACE_TABLE[0][v][2]) * 16u,
								BLOCK_TEXTURES[block.blockType][0],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][0],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][1],
								LIGHT[0]
							});
						}
					}

					// Bottom Face
					if (!(j != 0 ?
						_transparency[_index - CHUNK_SIZE] :
						IS_SOLID[chunkDown->getBlock(ChunkLocalBlockPos(i, CHUNK_SIZE - 1, k)).blockType]))
					{
						addFaceOdd(indicies, indexCounter, triangleCount);
						for (int v = 0; v < 4; ++v)
						{
							verticies.push_back({
								static_cast<uint16_t>(i + FACE_TABLE[1][v][0]) * 16u,
								static_cast<uint16_t>(j + FACE_TABLE[1][v][1]) * 16u,
								static_cast<uint16_t>(k + FACE_TABLE[1][v][2]) * 16u,
								BLOCK_TEXTURES[block.blockType][1],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][0],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][1],
								LIGHT[1]
							});
						}
					}

					// North face
					if (!(i != CHUNK_SIZE - 1 ?
						_transparency[_index + CHUNK_AREA] :
						IS_SOLID[chunkNorth->getBlock(ChunkLocalBlockPos(0, j, k)).blockType]))
					{
						addFaceEven(indicies, indexCounter, triangleCount);
						for (int v = 0; v < 4; ++v)
						{
							verticies.push_back(Vertex{
								static_cast<uint16_t>(i + FACE_TABLE[2][v][0]) * 16u,
								static_cast<uint16_t>(j + FACE_TABLE[2][v][1]) * 16u,
								static_cast<uint16_t>(k + FACE_TABLE[2][v][2]) * 16u,
								BLOCK_TEXTURES[block.blockType][2],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][0],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][1],
								LIGHT[2]
							});
						}
					}

					// South face
					if (!(i != 0 ?
						_transparency[_index - CHUNK_AREA] :
						IS_SOLID[chunkSouth->getBlock(ChunkLocalBlockPos(CHUNK_SIZE - 1, j, k)).blockType]))
					{
						addFaceOdd(indicies, indexCounter, triangleCount);
						for (int v = 0; v < 4; ++v)
						{
							verticies.push_back({
								static_cast<uint16_t>(i + FACE_TABLE[3][v][0]) * 16u,
								static_cast<uint16_t>(j + FACE_TABLE[3][v][1]) * 16u,
								static_cast<uint16_t>(k + FACE_TABLE[3][v][2]) * 16u,
								BLOCK_TEXTURES[block.blockType][3],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][0],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][1],
								LIGHT[3]
							});
						}
					}

					// East face
					if (!(k != CHUNK_SIZE - 1 ?
						_transparency[_index + 1] :
						IS_SOLID[chunkEast->getBlock(ChunkLocalBlockPos(i, j, 0)).blockType]))
					{
						addFaceEven(indicies, indexCounter, triangleCount);
						for (int v = 0; v < 4; ++v)
						{
							verticies.push_back({
								static_cast<uint16_t>(i + FACE_TABLE[4][v][0]) * 16u,
								static_cast<uint16_t>(j + FACE_TABLE[4][v][1]) * 16u,
								static_cast<uint16_t>(k + FACE_TABLE[4][v][2]) * 16u,
								BLOCK_TEXTURES[block.blockType][4],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][0],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][1],
								LIGHT[4]
							});
						}
					}

					// West face
					if (!(k != 0 ?
						_transparency[_index - 1] :
						IS_SOLID[chunkWest->getBlock(ChunkLocalBlockPos(i, j, CHUNK_SIZE - 1)).blockType]))
					{
						addFaceOdd(indicies, indexCounter, triangleCount);
						for (int v = 0; v < 4; ++v)
						{
							verticies.push_back({
								static_cast<uint16_t>(i + FACE_TABLE[5][v][0]) * 16u,
								static_cast<uint16_t>(j + FACE_TABLE[5][v][1]) * 16u,
								static_cast<uint16_t>(k + FACE_TABLE[5][v][2]) * 16u,
								BLOCK_TEXTURES[block.blockType][5],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][0],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][1],
								LIGHT[5]
							});
						}
					}
				}
				// Cross shaped plant
				else
				{
					ChunkLocalBlockPos localPos(i, j, k);

					uint16_t _dU = static_cast<uint16_t>(localPos.y + 1) * 16u;
					uint16_t _dD = static_cast<uint16_t>(localPos.y)     * 16u;
					uint16_t _dN = static_cast<uint16_t>(localPos.x + 1) * 16u;
					uint16_t _dS = static_cast<uint16_t>(localPos.x)     * 16u;
					uint16_t _dE = static_cast<uint16_t>(localPos.z + 1) * 16u;
					uint16_t _dW = static_cast<uint16_t>(localPos.z)     * 16u;
					uint16_t _tex = BLOCK_TEXTURES[block.blockType][0];

					verticies.push_back({ _dS, _dU, _dE, _tex,   0,   0, 255 });
					verticies.push_back({ _dN, _dU, _dW, _tex, 255,   0, 255 });
					verticies.push_back({ _dN, _dD, _dW, _tex, 255, 255, 255 });
					verticies.push_back({ _dS, _dD, _dE, _tex,   0, 255, 255 });

					verticies.push_back({ _dS, _dU, _dW, _tex,   0,   0, 255 });
					verticies.push_back({ _dN, _dU, _dE, _tex, 255,   0, 255 });
					verticies.push_back({ _dN, _dD, _dE, _tex, 255, 255, 255 });
					verticies.push_back({ _dS, _dD, _dW, _tex,   0, 255, 255 });

					for (int l = 0; l < 2; ++l)
					{
						indicies.push_back(indexCounter);
						indicies.push_back(indexCounter + 2);
						indicies.push_back(indexCounter + 1);
						indicies.push_back(indexCounter + 2);
						indicies.push_back(indexCounter);
						indicies.push_back(indexCounter + 3);

						indicies.push_back(indexCounter);
						indicies.push_back(indexCounter + 1);
						indicies.push_back(indexCounter + 2);
						indicies.push_back(indexCounter + 2);
						indicies.push_back(indexCounter + 3);
						indicies.push_back(indexCounter);
						indexCounter += 4;
					}
					
					triangleCount += 8;
				}
			}
		}
	}
}