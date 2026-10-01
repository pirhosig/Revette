#include "Renderer.h"
#include <algorithm>

#include "GlobalLog.h"
#include "Core/RevetteCore.h"

using namespace std::chrono_literals;



constexpr u32 RENDER_AHEAD_COUNT = 3;



void Renderer::processFrame() {
	globalApplicationState.rendererMeshIngestQueue.drain(std::back_inserter(incomingMeshes));
	auto playerPosition = globalApplicationState.playerPosition.load();
	frameRenderers[currentFrameRendererIndex].drawFrame(
		std::move(loadMeshQueue),
		playerPosition,
		meshesChunk
	);
	unloadMeshes();
	
	currentFrameRendererIndex = (currentFrameRendererIndex + 1) % frameRenderers.size();
}



// Need to defer deletion
void Renderer::unloadMeshes() {
	std::queue<ChunkPos> removeQueue;
	
	auto playerChunkPos = globalApplicationState.playerChunkPosition.load();
	ChunkPos2D playerChunkPos2D(playerChunkPos);

	const i64 _loadDistanceHorizontalSquared = (
		static_cast<i64>(settings.getLoadDistanceHorizontal()) *
		settings.getLoadDistanceHorizontal()
	);

	auto it = meshesChunk.begin();
	while (it != meshesChunk.end()) {
		if (
			playerChunkPos2D.distanceEuclideanSquared(it->first) > _loadDistanceHorizontalSquared ||
			std::abs(playerChunkPos.getY() - it->first.getY()) > settings.getLoadDistanceVertical()
		) {
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
	GlobalApplicationState& _globalApplicationState,
	const Settings& _settings,
	GLFWwindow* _window
) :
	globalApplicationState{_globalApplicationState},
	settings{_settings},
	nextTickTimestamp{std::chrono::steady_clock::now()},

	window{_window},
	vulkanContext(
		window,
		settings.getValidationLayersEnabled()
	),
	renderTarget(
		window,
		vulkanContext
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
	)
{
	frameRenderers.reserve(RENDER_AHEAD_COUNT);
	for (u32 i = 0; i < RENDER_AHEAD_COUNT; ++i) {
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
	while (globalApplicationState.applicationShouldTerminate.load() == false) {
		if (std::chrono::steady_clock::now() > nextTickTimestamp) {
			nextTickTimestamp += 25ms;
			globalApplicationState.currentGameClockTick++;
			globalApplicationState.currentGameClockTick.notify_all();
		}
		
		processFrame();
	}
}
