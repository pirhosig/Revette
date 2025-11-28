#pragma once
#include <memory>
#include <unordered_map>

#include "ChunkRenderer.h"
#include "FrameRenderer.h"
#include "RenderResources.h"
#include "RenderTarget.h"
#include "VulkanContext.h"
#include "Mesh/MeshChunk.h"
#include "../Threading/ThreadPointerQueue.h"
#include "../Window.h"
#include "../World/ChunkPos.h"
#include "../World/ChunkPosHash.h"
#include "../World/Entities/EntityPosition.h"




class Renderer
{
private:
	GLFWwindow* window;
	VulkanContext vulkanContext;
    RenderTarget renderTarget;
    RenderResources renderResources;
    ChunkRenderer renderProgram;

    std::vector<FrameRenderer> frameRenderers;

	// Mesh queue and storage
	std::shared_ptr<ThreadPointerQueue<MeshChunk::Data>> threadQueueMeshes;
	std::shared_ptr<ThreadQueue<ChunkPos>> threadQueueMeshDeletion;
	std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>> meshesChunk;

	size_t currentFrameRendererIndex = 0;

private:
	void unloadMeshes(const ChunkPos& playerChunk);
	
public:
	Renderer(
		GLFWwindow* window,
		std::shared_ptr<ThreadPointerQueue<MeshChunk::Data>> chunkMeshQueue,
		std::shared_ptr<ThreadQueue<ChunkPos>> chunkMeshQueueDeletion
	);
	~Renderer();

	void render(const EntityPosition& playerPos);
};
