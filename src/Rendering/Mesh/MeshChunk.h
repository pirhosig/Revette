#pragma once
#include <memory>
#include <vector>

#include "Core/RevetteCore.h"
#include "../Buffer.h"
#include "../LinearBufferSuballocator.h"
#include "../Vulkan_Headers.h"
#include "../../World/ChunkPos.h"
class Chunk;



class MeshChunk {
public:
	struct alignas(8) Vertex {
		u32 x: 10;
		u32 y: 10;
		u32 z: 10;
		u32 u: 1;
		u32 v: 1;
		u16 texture;
		u8  light;

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
	std::vector<u32> indices;

	u32 indexCountOpaque{};
	u32 indexCountTested{};
	u32 indexCountBlended{};

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
