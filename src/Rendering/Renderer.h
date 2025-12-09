#pragma once
#include <memory>
#include <unordered_map>

#include "ChunkRenderer.h"
#include "FrameRenderer.h"
#include "GuiRenderer.h"
#include "RenderResources.h"
#include "RenderTarget.h"
#include "VulkanContext.h"
#include "Mesh/MeshChunk.h"
#include "../Threading/SharedGameRendererState.h"
#include "../Window.h"
#include "../World/ChunkPos.h"
#include "../World/ChunkPosHash.h"
#include "../World/Entities/EntityPosition.h"




class Renderer
{
private:
	// Vulkan Stuff
	GLFWwindow* window;
	VulkanContext vulkanContext;
    RenderTarget renderTarget;
    RenderResources renderResources;
    ChunkRenderer chunkRenderer;
	GuiRenderer guiRenderer;

    std::vector<FrameRenderer> frameRenderers;
	
	size_t currentFrameRendererIndex = 0;

	// Drawables
	std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>> meshesChunk;

	// Threading Stuff
	std::atomic_bool& applicationShouldTerminate;
	std::shared_ptr<SharedGameRendererState> sharedGameState;

	// User Input
	double cursorLastX;
	double cursorLastY;

private:
	void processFrame();
	void unloadMeshes(const ChunkPos& playerChunk);
	
public:
	Renderer(
		GLFWwindow* _window,
		std::atomic_bool& _applicationShouldTerminate,
		std::shared_ptr<SharedGameRendererState> _sharedGameState
	);
	~Renderer();

	void run();
};
