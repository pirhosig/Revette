#pragma once
#include <GLFW/glfw3.h>

#include "LoopTimer.h"
#include "Rendering/Renderer.h"
#include "World/World.h"


class LoopGame
{
public:
	LoopGame(
		GLFWwindow* _window,
		std::shared_ptr<ThreadPointerQueue<MeshChunk::Data>> chunkMeshQueue,
		std::shared_ptr<ThreadQueue<ChunkPos>> chunkMeshQueueDeletion
	);

	void run();

	static void cursorPositionCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
private:
	void processInput(const double deltaTime);

	void cursorPositionCallback(double xpos, double ypos);

	LoopTimer timerFramerate;

	double cursorLastX;
	double cursorLastY;

	Renderer renderer;
	World world;

	Entity player;

	GLFWwindow* window;
};