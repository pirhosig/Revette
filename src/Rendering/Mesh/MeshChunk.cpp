#include "MeshChunk.h"
#include <numbers>
#include <string>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "../../World/World.h"



std::array<VkVertexInputBindingDescription, 1> MeshChunk::Vertex::getBindingDescriptions() {
    return {
        VkVertexInputBindingDescription{
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        }
    };
}



std::array<VkVertexInputAttributeDescription, 3> MeshChunk::Vertex::getAttributeDescriptions() {
    return {
        VkVertexInputAttributeDescription{
            .location = 0,
            .binding = 0,
            .format = VK_FORMAT_A2B10G10R10_UINT_PACK32,
            .offset = 0
        },
        VkVertexInputAttributeDescription{
            .location = 1,
            .binding = 0,
            .format = VK_FORMAT_R16_USCALED,
            .offset = offsetof(Vertex, texture)
        },
        VkVertexInputAttributeDescription{
            .location = 2,
            .binding = 0,
            .format = VK_FORMAT_R8_UNORM,
            .offset = offsetof(Vertex, light)
        },
    };
}



namespace {
	template <typename T>
	VkDeviceSize getVectorByteSize(const std::vector<T>& v) {
		return sizeof(T) * v.size();
	}



	uint32_t basicHash(uint32_t x)
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
}



constexpr uint16_t BLOCK_TEXTURES[][6] = {
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
	{18, 18, 18, 18, 18, 18}
};
constexpr bool IS_SOLID[] = {
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
constexpr int MESH_TYPE[] = {
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
constexpr bool IS_ROTATEABLE[] = {
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
constexpr uint8_t TEXTURE_COORDINATES[4][2] = {
	{ 0, 0 },
	{ 1, 0 },
	{ 1, 1 },
	{ 0, 1 }
};
// Baked lighting to make block edges visible.
// This is a rather horrible hack but shall stay until a proper light system exists.
constexpr uint8_t LIGHT[6] = { 255, 229, 240, 240, 220, 220 };



MeshChunk::Data::Data(const Chunk* chunkCentre, const std::array<Chunk*, 6> neighbours)
 : position(chunkCentre->position)
{
	// Skip loop if chunk is empty
	if (chunkCentre->isEmpty()) return;

	// Cache transparency
	auto _trans = chunkCentre->blockContainer.getSolid();
	std::array<std::vector<bool>, 6> neighbourSolidMasks;
	for (unsigned i = 0; i < 6; ++i) {
		neighbourSolidMasks[i] = neighbours[i]->getSolidFaceMask(static_cast<AxisDirection>(i ^ 1));
	}

	// Temporary storage for vertices which gets merged together at the end
	std::vector<Vertex> _verticesOpaque;
	std::vector<Vertex> _verticesTested;
	std::vector<Vertex> _verticesBlended;

	std::vector<uint32_t> _indicesOpaque;
	std::vector<uint32_t> _indicesTested;
	std::vector<uint32_t> _indicesBlended;

	// Loop and check for each block whether it is solid, and so whether it needs to be added
	for (int i = 0; i < CHUNK_SIZE; ++i) {
	for (int j = 0; j < CHUNK_SIZE; ++j) {
	for (int k = 0; k < CHUNK_SIZE; ++k) {
		int _index = i * CHUNK_AREA + j * CHUNK_SIZE + k;
		Block block = chunkCentre->blockContainer.getBlockRaw(_index);
		// Skip if air block
		if (block.blockType == 0) continue;

		switch (MESH_TYPE[block.blockType])
		{
		// Solid cube, the most basic and common mesh type
		case 0: [[likely]]
		{
			// Offsets for every vertex to draw a cube
			constexpr int FACE_TABLE[6][4][3] = {
				{{ 0, 1, 0 }, { 1, 1, 0 }, { 1, 1, 1 }, { 0, 1, 1 }}, // Up
				{{ 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 }}, // Down
				{{ 1, 1, 1 }, { 1, 1, 0 }, { 1, 0, 0 }, { 1, 0, 1 }}, // North
				{{ 0, 1, 1 }, { 0, 1, 0 }, { 0, 0, 0 }, { 0, 0, 1 }}, // South
				{{ 0, 1, 1 }, { 1, 1, 1 }, { 1, 0, 1 }, { 0, 0, 1 }}, // East
				{{ 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 }, { 0, 0, 0 }}  // West
			};
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
					uint32_t baseIndex = static_cast<uint32_t>(_verticesOpaque.size());

					for (int v = 0; v < 4; ++v)
					{
						_verticesOpaque.push_back(Vertex{
							.x = static_cast<uint16_t>(i + FACE_TABLE[l][v][0]) * 16u,
							.y = static_cast<uint16_t>(j + FACE_TABLE[l][v][1]) * 16u,
							.z = static_cast<uint16_t>(k + FACE_TABLE[l][v][2]) * 16u,
							.u = TEXTURE_COORDINATES[(v + rotationOffset) % 4][0],
							.v = TEXTURE_COORDINATES[(v + rotationOffset) % 4][1],
							.texture = BLOCK_TEXTURES[block.blockType][l],
							.light = LIGHT[l]
						});
					}
					
					if (l & 1)
					{
						_indicesOpaque.push_back(baseIndex + 0);
						_indicesOpaque.push_back(baseIndex + 1);
						_indicesOpaque.push_back(baseIndex + 2);
						_indicesOpaque.push_back(baseIndex + 2);
						_indicesOpaque.push_back(baseIndex + 3);
						_indicesOpaque.push_back(baseIndex + 0);
					}
					else
					{
						_indicesOpaque.push_back(baseIndex + 0);
						_indicesOpaque.push_back(baseIndex + 2);
						_indicesOpaque.push_back(baseIndex + 1);
						_indicesOpaque.push_back(baseIndex + 2);
						_indicesOpaque.push_back(baseIndex + 0);
						_indicesOpaque.push_back(baseIndex + 3);
					}
				}
		}
			break;
		// Cross shaped plant
		case 1: [[unlikely]]
		{
			ChunkLocalBlockPos localPos(i, j, k);

			uint16_t _dU = static_cast<uint16_t>(localPos.y + 1) * 16u;
			uint16_t _dD = static_cast<uint16_t>(localPos.y)     * 16u;
			uint16_t _dN = static_cast<uint16_t>(localPos.x + 1) * 16u;
			uint16_t _dS = static_cast<uint16_t>(localPos.x)     * 16u;
			uint16_t _dE = static_cast<uint16_t>(localPos.z + 1) * 16u;
			uint16_t _dW = static_cast<uint16_t>(localPos.z)     * 16u;
			uint16_t _tex = BLOCK_TEXTURES[block.blockType][0];

			uint32_t baseIndex = static_cast<uint32_t>(_verticesTested.size());

			_verticesTested.push_back(Vertex{
				.x = _dS,
				.y = _dU,
				.z = _dE,
				.u = 0,
				.v = 0,
				.texture = _tex,
				.light = 255
			});
			_verticesTested.push_back(Vertex{
				.x = _dN,
				.y = _dU,
				.z = _dW,
				.u = 1,
				.v = 0,
				.texture = _tex,
				.light = 255
			});
			_verticesTested.push_back(Vertex{
				.x = _dN,
				.y = _dD,
				.z = _dW,
				.u = 1,
				.v = 1,
				.texture = _tex,
				.light = 255
			});
			_verticesTested.push_back(Vertex{
				.x = _dS,
				.y = _dD,
				.z = _dE,
				.u = 0,
				.v = 1,
				.texture = _tex,
				.light = 255
			});

			_verticesTested.push_back(Vertex{
				.x = _dS,
				.y = _dU,
				.z = _dW,
				.u = 0,
				.v = 0,
				.texture = _tex,
				.light = 255
			});
			_verticesTested.push_back(Vertex{
				.x = _dN,
				.y = _dU,
				.z = _dE,
				.u = 1,
				.v = 0,
				.texture = _tex,
				.light = 255
			});
			_verticesTested.push_back(Vertex{
				.x = _dN,
				.y = _dD,
				.z = _dE,
				.u = 1,
				.v = 1,
				.texture = _tex,
				.light = 255
			});
			_verticesTested.push_back(Vertex{
				.x = _dS,
				.y = _dD,
				.z = _dW,
				.u = 0,
				.v = 1,
				.texture = _tex,
				.light = 255
			});

			// This is so ass.
			_indicesTested.push_back(baseIndex + 0);
			_indicesTested.push_back(baseIndex + 2);
			_indicesTested.push_back(baseIndex + 1);
			_indicesTested.push_back(baseIndex + 2);
			_indicesTested.push_back(baseIndex + 0);
			_indicesTested.push_back(baseIndex + 3);
			_indicesTested.push_back(baseIndex + 0);
			_indicesTested.push_back(baseIndex + 1);
			_indicesTested.push_back(baseIndex + 2);
			_indicesTested.push_back(baseIndex + 2);
			_indicesTested.push_back(baseIndex + 3);
			_indicesTested.push_back(baseIndex + 0);

			_indicesTested.push_back(baseIndex + 4);
			_indicesTested.push_back(baseIndex + 6);
			_indicesTested.push_back(baseIndex + 5);
			_indicesTested.push_back(baseIndex + 6);
			_indicesTested.push_back(baseIndex + 4);
			_indicesTested.push_back(baseIndex + 7);
			_indicesTested.push_back(baseIndex + 4);
			_indicesTested.push_back(baseIndex + 5);
			_indicesTested.push_back(baseIndex + 6);
			_indicesTested.push_back(baseIndex + 6);
			_indicesTested.push_back(baseIndex + 7);
			_indicesTested.push_back(baseIndex + 4);
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
			
			uint32_t baseIndex = static_cast<uint32_t>(_verticesBlended.size());

			for (int l = 0; l < 2; ++l)
			{
				for (int v = 0; v < 4; ++v)
				{
					_verticesBlended.push_back(Vertex{
						.x = static_cast<uint16_t>(i + FACE_TABLE[v][0]) * 16u,
						.y = static_cast<uint16_t>(j) * 16u + 13u,
						.z = static_cast<uint16_t>(k + FACE_TABLE[v][1]) * 16u,
						.u = TEXTURE_COORDINATES[(v + rotationOffset) % 4][0],
						.v = TEXTURE_COORDINATES[(v + rotationOffset) % 4][1],
						.texture = BLOCK_TEXTURES[block.blockType][l],
						.light = LIGHT[l]
					});
				}
			}
			
			_indicesBlended.push_back(baseIndex + 0);
			_indicesBlended.push_back(baseIndex + 2);
			_indicesBlended.push_back(baseIndex + 1);
			_indicesBlended.push_back(baseIndex + 2);
			_indicesBlended.push_back(baseIndex + 0);
			_indicesBlended.push_back(baseIndex + 3);
			_indicesBlended.push_back(baseIndex + 4);
			_indicesBlended.push_back(baseIndex + 5);
			_indicesBlended.push_back(baseIndex + 6);
			_indicesBlended.push_back(baseIndex + 6);
			_indicesBlended.push_back(baseIndex + 7);
			_indicesBlended.push_back(baseIndex + 4);
		}
			break;
		// Nah shit's gone wrong if this is triggered
		default:
			throw std::runtime_error("Meshing error: unkown mesh type");
		}
	}
	}
	}

	indexCountOpaque = _indicesOpaque.size();
	indexCountTested = _indicesTested.size();
	indexCountBlended = _indicesBlended.size();

	// Merge the vertex and index vectors into one
	vertices = std::move(_verticesOpaque);
	vertices.reserve(vertices.size() + _verticesTested.size() + _verticesBlended.size());
	vertices.insert(vertices.end(), _verticesTested.begin(), _verticesTested.end());
	vertices.insert(vertices.end(), _verticesBlended.begin(), _verticesBlended.end());

	indices = std::move(_indicesOpaque);
	indices.reserve(indices.size() + _indicesTested.size() + _indicesBlended.size());
	indices.insert(indices.end(), _indicesTested.begin(), _indicesTested.end());
	indices.insert(indices.end(), _indicesBlended.begin(), _indicesBlended.end());
}



bool MeshChunk::Data::isEmpty() const {
	return indices.empty();
}



ChunkPos MeshChunk::Data::getPosition() const {
	return position;
}



// Creates a chunk mesh using the passed data object, takes ownership of the meshData object
MeshChunk::MeshChunk(
	VkBufferMemoryBarrier2& barrier,
	std::unique_ptr<MeshChunk::Data> _meshData,
	VmaAllocator allocator,
	VkCommandBuffer transferCommandBuffer,
	LinearBufferSuballocator& stagingBuffer
) :
	meshData{std::move(_meshData)},
	buffer(
		allocator,
		getVectorByteSize(meshData->vertices) + getVectorByteSize(meshData->indices),
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		{},
		VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
		{}
	)
{
	VkDeviceSize sizeVertices = getVectorByteSize(meshData->vertices);
	VkDeviceSize sizeIndices = getVectorByteSize(meshData->indices);


	// TODO add a path for ReBAR which writes directly to the buffer


	

	// Write data to staging buffer, and copy it into the buffer
	VkDeviceSize stagingOfsetVertices = stagingBuffer.writeData(
		meshData->vertices.data(),
		sizeVertices
	);
	offsetVertices = 0;

	static_cast<void>(stagingBuffer.writeData(
		meshData->indices.data(),
		sizeIndices
	));
	offsetIndices = sizeVertices;

	VkBufferCopy copyRegion{
		.srcOffset = stagingOfsetVertices,
		.dstOffset = 0,
		.size = sizeVertices + sizeIndices
	};
	vkCmdCopyBuffer(
		transferCommandBuffer,
		stagingBuffer.getHandle(),
		buffer.getHandle(),
		1,
		&copyRegion
	);

	barrier = VkBufferMemoryBarrier2{
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
        .pNext{},
        .srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT,
        .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
        .dstAccessMask = VK_ACCESS_2_INDEX_READ_BIT | VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT,
        .srcQueueFamilyIndex{},
		.dstQueueFamilyIndex{},
		.buffer = buffer.getHandle(),
		.offset{},
		.size = VK_WHOLE_SIZE
    };
}



namespace {

void startDraw(
	VkCommandBuffer commandBuffer,
	VkPipelineLayout pipelineLayout,
	const glm::mat4& matrixProjectionView,
	ChunkOffset offset,
	VkBuffer buffer,
	VkDeviceSize offsetVertices,
	VkDeviceSize offsetIndices
) {
	glm::mat4 matrixMVP = matrixProjectionView * glm::translate(
		glm::mat4(1.0f),
		glm::vec3(offset.x * CHUNK_SIZE, offset.y * CHUNK_SIZE, offset.z * CHUNK_SIZE) * 0.5f
	);

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, &offsetVertices);
	vkCmdBindIndexBuffer(commandBuffer, buffer, offsetIndices, VK_INDEX_TYPE_UINT32);

	vkCmdPushConstants(
		commandBuffer,
		pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT,
		0,
		sizeof(matrixMVP),
		&matrixMVP[0][0]
	);
}

}


void MeshChunk::drawOpaque(
	VkCommandBuffer commandBuffer,
	VkPipelineLayout pipelineLayout,
	const glm::mat4& matrixProjectionView,
	ChunkPos playerPosition
) const {
	if (meshData->indexCountOpaque == 0) return;
	startDraw(
		commandBuffer,
		pipelineLayout,
		matrixProjectionView,
		playerPosition.offset(meshData->position),
		buffer.getHandle(),
		offsetVertices,
		offsetIndices
	);
	vkCmdDrawIndexed(commandBuffer, meshData->indexCountOpaque, 1, 0, 0, 0);
}



void MeshChunk::drawTested(
	VkCommandBuffer commandBuffer,
	VkPipelineLayout pipelineLayout,
	const glm::mat4& matrixProjectionView,
	ChunkPos playerPosition
) const {
	if (meshData->indexCountTested == 0) return;
	startDraw(
		commandBuffer,
		pipelineLayout,
		matrixProjectionView,
		playerPosition.offset(meshData->position),
		buffer.getHandle(),
		offsetVertices,
		offsetIndices
	);
	vkCmdDrawIndexed(
		commandBuffer,
		meshData->indexCountTested,
		1,
		meshData->indexCountOpaque,
		(meshData->indexCountOpaque / 3) * 2,
		0
	);
}



void MeshChunk::drawBlended(
	VkCommandBuffer commandBuffer,
	VkPipelineLayout pipelineLayout,
	const glm::mat4& matrixProjectionView,
	ChunkPos playerPosition
) const {
	if (meshData->indexCountBlended == 0) return;
	startDraw(
		commandBuffer,
		pipelineLayout,
		matrixProjectionView,
		playerPosition.offset(meshData->position),
		buffer.getHandle(),
		offsetVertices,
		offsetIndices
	);
	vkCmdDrawIndexed(
		commandBuffer,
		meshData->indexCountBlended,
		1,
		meshData->indexCountOpaque + meshData->indexCountTested,
		((meshData->indexCountOpaque + meshData->indexCountTested) / 3) * 2,
		0
	);
}



ChunkPos MeshChunk::getPosition() const {
	return meshData->position;
}


