#include "MeshDataChunk.h"

#include "../../World/World.h"
#include "../../World/Chunk.h"



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
	false,
	true,
	true,
	true,
	false,
	true,
	false,
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
	true
};



int MESH_TYPE[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	2,
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



MeshDataChunk::MeshDataChunk(const Chunk* chunkCentre, const std::array<Chunk*, 6> neighbours)
 : position(chunkCentre->position), triangleCountOpaque{ 0 }, triangleCountTransparent{ 0 }
{
	int indexCounterOpaque = 0;
	int indexCounterTransparent = 0;

	// Skip loop if chunk is empty
	if (chunkCentre->isEmpty()) return;

	// Cache transparency
	auto _trans = chunkCentre->blockContainer.getSolid();
	std::array<std::vector<bool>, 6> neighbourSolidMasks;
	for (unsigned i = 0; i < 6; ++i)
		neighbourSolidMasks[i] = neighbours[i]->getSolidFaceMask(static_cast<AxisDirection>(i ^ 1));

	std::vector<Vertex> _verticiesTrans;
	std::vector<uint32_t> _indiciesTrans;

	// Loop and check for each block whether it is solid, and so whether it needs to be added
	for (int i = 0; i < CHUNK_SIZE; ++i)
		for (int j = 0; j < CHUNK_SIZE; ++j)
			for (int k = 0; k < CHUNK_SIZE; ++k)
			{
				Block block = chunkCentre->getBlock(ChunkLocalBlockPos(i, j, k));
				// Skip if air block
				if (block.blockType == 0) continue;

				switch (MESH_TYPE[block.blockType])
				{
				// Solid cube, the most basic and common mesh type
				case 0: [[likely]]
				{
					// Offsets for every vertex to draw a cube
					const int FACE_TABLE[6][4][3] = {
						{{ 0, 1, 0 }, { 1, 1, 0 }, { 1, 1, 1 }, { 0, 1, 1 }}, // Up
						{{ 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 }}, // Down
						{{ 1, 1, 1 }, { 1, 1, 0 }, { 1, 0, 0 }, { 1, 0, 1 }}, // North
						{{ 0, 1, 1 }, { 0, 1, 0 }, { 0, 0, 0 }, { 0, 0, 1 }}, // South
						{{ 0, 1, 1 }, { 1, 1, 1 }, { 1, 0, 1 }, { 0, 0, 1 }}, // East
						{{ 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 }, { 0, 0, 0 }}  // West
					};

					int _index = i * CHUNK_AREA + j * CHUNK_SIZE + k;
					int rotationOffset = IS_ROTATEABLE[block.blockType] ?
						static_cast<int>(getPositionHash(ChunkLocalBlockPos(i, j, k).asBlockPos(position), basicHash(1)) % 4) : 0;
					bool faceIsVisible[6] = {
						!(j != CHUNK_SIZE - 1 ? _trans[_index + CHUNK_SIZE] : neighbourSolidMasks[0][i * CHUNK_SIZE + k]),
						!(j != 0 ? _trans[_index - CHUNK_SIZE] : neighbourSolidMasks[1][i * CHUNK_SIZE + k]),
						!(i != CHUNK_SIZE - 1 ? _trans[_index + CHUNK_AREA] : neighbourSolidMasks[2][j * CHUNK_SIZE + k]),
						!(i != 0 ? _trans[_index - CHUNK_AREA] : neighbourSolidMasks[3][j * CHUNK_SIZE + k]),
						!(k != CHUNK_SIZE - 1 ? _trans[_index + 1] : neighbourSolidMasks[4][i * CHUNK_SIZE + j]),
						!(k != 0 ? _trans[_index - 1] : neighbourSolidMasks[5][i * CHUNK_SIZE + j])
					};
					// Loop over each of the block faces
					for (int l = 0; l < 6; ++l)
						if (faceIsVisible[l])
						{
							for (int v = 0; v < 4; ++v)
								verticies.push_back({
									static_cast<uint16_t>(i + FACE_TABLE[l][v][0]) * 16u,
									static_cast<uint16_t>(j + FACE_TABLE[l][v][1]) * 16u,
									static_cast<uint16_t>(k + FACE_TABLE[l][v][2]) * 16u,
									BLOCK_TEXTURES[block.blockType][l],
									TEXTURE_COORDINATES[(v + rotationOffset) % 4][0],
									TEXTURE_COORDINATES[(v + rotationOffset) % 4][1],
									LIGHT[l]
							});
							if (l % 2) for (int a : {0, 1, 2, 2, 3, 0}) indicies.push_back(indexCounterOpaque + a);
							else       for (int a : {0, 2, 1, 2, 0, 3}) indicies.push_back(indexCounterOpaque + a);
							indexCounterOpaque += 4;
							triangleCountOpaque += 2;
						}
				}
					break;
				// Cross shaped plant
				case 1: [[unlikely]]
				{
					ChunkLocalBlockPos localPos(i, j, k);

					uint16_t _dU = static_cast<uint16_t>(localPos.y + 1) * 16u;
					uint16_t _dD = static_cast<uint16_t>(localPos.y) * 16u;
					uint16_t _dN = static_cast<uint16_t>(localPos.x + 1) * 16u;
					uint16_t _dS = static_cast<uint16_t>(localPos.x) * 16u;
					uint16_t _dE = static_cast<uint16_t>(localPos.z + 1) * 16u;
					uint16_t _dW = static_cast<uint16_t>(localPos.z) * 16u;
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
						for (int a : {0, 2, 1, 2, 0, 3, 0, 1, 2, 2, 3, 0}) indicies.push_back(indexCounterOpaque + a);
						indexCounterOpaque += 4;
					}
					triangleCountOpaque += 8;
				}
					break;
				// Water
				case 2:
				{
					// Skip if block above is same type
					if (((j != CHUNK_SIZE - 1) ? chunkCentre->getBlock(ChunkLocalBlockPos(i, j + 1, k)).blockType :
						neighbours[0]->getBlock(ChunkLocalBlockPos(i, 0, k)).blockType) == block.blockType) continue;

					int rotationOffset = IS_ROTATEABLE[block.blockType] ?
						static_cast<int>(getPositionHash(ChunkLocalBlockPos(i, j, k).asBlockPos(position), basicHash(1)) % 4) : 0;

					const uint16_t FACE_TABLE[4][2] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
					
					for (int l = 0; l < 2; ++l)
						for (int v = 0; v < 4; ++v)
							_verticiesTrans.push_back({
								static_cast<uint16_t>(i + FACE_TABLE[v][0]) * 16u,
								static_cast<uint16_t>(j) * 16u + 13u,
								static_cast<uint16_t>(k + FACE_TABLE[v][1]) * 16u,
								BLOCK_TEXTURES[block.blockType][l],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][0],
								TEXTURE_COORDINATES[(v + rotationOffset) % 4][1],
								LIGHT[l]
						});
					for (int a : {0, 2, 1, 2, 0, 3, 4, 5, 6, 6, 7, 4}) _indiciesTrans.push_back(indexCounterTransparent + a);
					indexCounterTransparent += 8;
					triangleCountTransparent += 4;
				}
					break;
				// Nah shit's gone wrong if this is triggered
				default:
					break;
				}
			}
	// Merge transparent verticies
	for (auto& vertex : _verticiesTrans) verticies.push_back(vertex);
	for (auto& index : _indiciesTrans) indicies.push_back(indexCounterOpaque + index);
}