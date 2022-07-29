#pragma once
#include <atomic>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "LoopTimer.h"
#include "Rendering/Mesh/MeshDataChunk.h"
#include "Rendering/ShaderProgram.h"
#include "Threading/PlayerState.h"
#include "Threading/ThreadPointerQueue.h"
#include "World/Entities/EntityPosition.h"



class RenderingLoop
{
public:
	RenderingLoop();
	~RenderingLoop();

	void runLoop(
		std::atomic<bool>& gameShouldClose,
		std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes,
		std::shared_ptr<ThreadQueue<ChunkPos>> threadQueueMeshDeletion,
		std::atomic<PlayerState>& playerState
	);

	static void cursorPositionCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
private:
	void processInput(const double deltaTime);


	// Callback functions

	double cursorLastX;
	double cursorLastY;
	void cursorPositionCallback(double xpos, double ypos);

	LoopTimer framerateCounter;
	EntityPosition playerPos;
	GLFWwindow* mainWindow;
};