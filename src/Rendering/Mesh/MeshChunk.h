#pragma once
#include <memory>
#include <vector>

#include "../Buffer.h"
#include "../LinearBufferSuballocator.h"
#include "../Vulkan_Headers.h"
#include "../../World/ChunkPos.h"
class Chunk;



class MeshChunk {
public:
	struct alignas(8) Vertex {
		uint32_t x: 10;
		uint32_t y: 10;
		uint32_t z: 10;
		uint32_t u: 1;
		uint32_t v: 1;
		uint16_t texture;
		uint8_t light;

		static std::array<VkVertexInputBindingDescription, 1> getBindingDescriptions();
		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
	};

	// In memory data class which can be used to construct a full MeshChunk which is backed by actual GPU buffers
	class Data;

private:
	std::unique_ptr<MeshChunk::Data> meshData;

	Buffer buffer;

	VkDeviceSize offsetVertices;
	VkDeviceSize offsetIndices;

public:
	MeshChunk(
		VkBufferMemoryBarrier2& barrier,
		std::unique_ptr<MeshChunk::Data> _meshData,
		VmaAllocator allocator,
		VkCommandBuffer transferCommandBuffer,
		LinearBufferSuballocator& stagingBuffer
	);

	MeshChunk(MeshChunk&&) = delete;
	MeshChunk(const MeshChunk&) = delete;
	MeshChunk operator=(MeshChunk&&) = delete;
	MeshChunk operator=(const MeshChunk&) = delete;

	void drawOpaque(
		VkCommandBuffer commandBuffer,
		VkPipelineLayout pipelineLayout,
		const glm::mat4& matrixProjectionView,
		ChunkPos playerPosition
	) const;
	void drawTested(
		VkCommandBuffer commandBuffer,
		VkPipelineLayout pipelineLayout,
		const glm::mat4& matrixProjectionView,
		ChunkPos playerPosition
	) const;
	void drawBlended(
		VkCommandBuffer commandBuffer,
		VkPipelineLayout pipelineLayout,
		const glm::mat4& matrixProjectionView,
		ChunkPos playerPosition
	) const;

	ChunkPos getPosition() const;
};



class MeshChunk::Data {
private:
	ChunkPos position;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	uint32_t indexCountOpaque{};
	uint32_t indexCountTested{};
	uint32_t indexCountBlended{};

public:
	Data(const Chunk* chunkCentre, const std::array<Chunk*, 6> neighbours);

	Data(Data&&) = delete;
	Data(const Data&) = delete;
	Data operator=(Data&&) = delete;
	Data operator=(const Data&) = delete;

	bool isEmpty() const;
	ChunkPos getPosition() const;

	friend MeshChunk;
};
