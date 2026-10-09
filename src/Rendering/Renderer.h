#pragma once
#include <memory>
#include <unordered_map>

#include "Util/efficient_vector.h"
#include "ChunkRenderer.h"
#include "FrameRenderer.h"
#include "GuiRenderer.h"
#include "RenderResources.h"
#include "RenderTarget.h"
#include "VulkanContext.h"
#include "Mesh/MeshChunk.h"
#include "../Settings.h"
#include "../Window.h"
#include "../Application/GlobalApplicationState.h"
#include "../World/ChunkPos.h"
#include "../World/Entities/EntityPosition.h"




class Renderer {
private:
	// External state objects
	GlobalApplicationState& globalApplicationState;
	const Settings& settings;
	std::chrono::steady_clock::time_point nextTickTimestamp;

	// Vulkan Stuff
	GLFWwindow* window;
	VulkanContext vulkanContext;
    RenderTarget renderTarget;
    RenderResources renderResources;
    ChunkRenderer chunkRenderer;
	GuiRenderer guiRenderer;

	// Drawing 
	std::vector<std::unique_ptr<MeshChunk::Data>> incomingMeshes;
	rvl::vector32<FrameRenderer> frameRenderers;
	
	size_t currentFrameRendererIndex = 0;

	// Drawables
	std::unordered_map<ChunkPos, std::unique_ptr<MeshChunk>> meshesChunk;

	// User Input
	double cursorLastX;
	double cursorLastY;

private:
	void processFrame();
	void unloadMeshes();
	
public:
	explicit Renderer(
		GlobalApplicationState& _globalApplicationState,
		const Settings& _settings,
		GLFWwindow* _window
	);
	~Renderer();

	void run();
};
