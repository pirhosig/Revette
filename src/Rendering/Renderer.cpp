#include "Renderer.h"
#include <algorithm>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../GlobalLog.h"



constexpr int RENDER_AHEAD_COUNT = 3;



namespace {

inline bool withinLoadDistance(ChunkPos _pos, ChunkPos _centre)
{
	auto _offset = _pos.offset(_centre);
	return ((_offset.x * _offset.x + _offset.z * _offset.z <= LOAD_DISTANCE * LOAD_DISTANCE) &&
		(std::abs(_offset.y) <= LOAD_DISTANCE_VERTICAL));
}



struct CmpChunkPos
{
	ChunkPos centre;

	bool operator()(const ChunkPos& a, const ChunkPos& b) const
	{
		return centre.distance(a) < centre.distance(b);
	}
};

}



Renderer::Renderer(
	GLFWwindow* _window,
	std::shared_ptr<ThreadPointerQueue<MeshChunk::Data>> chunkMeshQueue,
	std::shared_ptr<ThreadQueue<ChunkPos>> chunkMeshQueueDeletion
) :
	window{_window},
	vulkanContext(
		window,
		true
	),
	renderTarget(
		window,
		vulkanContext.getPhysicalDevice(),
		vulkanContext.getSurface(),
		vulkanContext.getDevice(),
		vulkanContext.getAllocator()
	),
	renderResources(
		vulkanContext.getDevice(),
		vulkanContext.getQueueGraphics(),
		vulkanContext.getQueueGraphicsFamily(),
		vulkanContext.getAllocator()
	),
	chunkRenderer(
		vulkanContext.getDevice(),
		renderTarget,
		renderResources.getDescriptorLayout()
	),
	guiRenderer(
		vulkanContext.getDevice(),
		renderTarget,
		renderResources.getDescriptorLayout()
	),
	threadQueueMeshes{ chunkMeshQueue },
	threadQueueMeshDeletion{ chunkMeshQueueDeletion }
{
	frameRenderers.reserve(RENDER_AHEAD_COUNT);
	for (int i = 0; i < RENDER_AHEAD_COUNT; ++i) {
        frameRenderers.emplace_back(
            vulkanContext.getDevice(),
            vulkanContext.getQueueGraphics(),
            renderTarget,
            renderResources,
            chunkRenderer,
			guiRenderer,
            vulkanContext.getQueueGraphicsFamily(),
			vulkanContext.getAllocator()
        );
    }

	GlobalLog.Write("Created renderer");
}



Renderer::~Renderer() {
	vulkanContext.waitDeviceIdle();
}



void Renderer::render(const EntityPosition& playerPos)
{
	std::queue<std::unique_ptr<MeshChunk::Data>> loadMeshQueue;
	threadQueueMeshes->getQueue(loadMeshQueue);
	frameRenderers[currentFrameRendererIndex].drawFrame(
		std::move(loadMeshQueue),
		playerPos,
		meshesChunk
	);
	unloadMeshes(ChunkPos(playerPos));

	currentFrameRendererIndex = (currentFrameRendererIndex + 1) % frameRenderers.size();
}



// Need to defer deletion
void Renderer::unloadMeshes(const ChunkPos& playerChunk)
{
	std::queue<ChunkPos> removeQueue;

	auto it = meshesChunk.begin();
	while (it != meshesChunk.end())
	{
		if (!withinLoadDistance(it->first, playerChunk))
		{
			removeQueue.push(it->first);
			frameRenderers[currentFrameRendererIndex].queueMeshForDeletion(std::move(it->second));
			it = meshesChunk.erase(it);
		}
		else it++;
	}

	// Add the removed chunks if any were removed
	if (removeQueue.size()) threadQueueMeshDeletion->mergeQueue(removeQueue);
}
