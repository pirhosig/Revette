#include "Renderer.h"
#include <algorithm>

#include "../GlobalLog.h"

using namespace std::chrono_literals;



constexpr int RENDER_AHEAD_COUNT = 3;



namespace {

bool withinLoadDistance(ChunkPos _pos, ChunkPos _centre) {
	auto _offset = _pos.offset(_centre);
	return (
		(_offset.x * _offset.x + _offset.z * _offset.z <= LOAD_DISTANCE * LOAD_DISTANCE) &&
		(std::abs(_offset.y) <= LOAD_DISTANCE_VERTICAL)
	);
}



struct CmpChunkPos {
	ChunkPos centre;

	bool operator()(const ChunkPos& a, const ChunkPos& b) const {
		return centre.distance(a) < centre.distance(b);
	}
};

}



void Renderer::processFrame() {
	std::queue<std::unique_ptr<MeshChunk::Data>> loadMeshQueue;
	sharedGameState->chunkMeshQueue->getQueue(loadMeshQueue);
	EntityPosition playerPos = sharedGameState->playerPosition.load();
	frameRenderers[currentFrameRendererIndex].drawFrame(
		std::move(loadMeshQueue),
		playerPos,
		meshesChunk
	);
	unloadMeshes(ChunkPos(playerPos));
	
	currentFrameRendererIndex = (currentFrameRendererIndex + 1) % frameRenderers.size();
}



// Need to defer deletion
void Renderer::unloadMeshes(const ChunkPos& playerChunk) {
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
	if (removeQueue.size()) sharedGameState->chunkMeshQueueDeletion->mergeQueue(removeQueue);
}



Renderer::Renderer(
	GLFWwindow* _window,
	std::atomic_bool& _applicationShouldTerminate,
	std::shared_ptr<SharedGameRendererState> _sharedGameState
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
	applicationShouldTerminate{_applicationShouldTerminate},
	sharedGameState{std::move(_sharedGameState)}
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



void Renderer::run() {
	while (applicationShouldTerminate.load() == false) {
		if (std::chrono::steady_clock::now() > sharedGameState->nextTickTimestamp) {
			sharedGameState->nextTickTimestamp += 20ms;
			sharedGameState->currentTick++;
			sharedGameState->currentTick.notify_all();
		}
		
		processFrame();
	}
}
