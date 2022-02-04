#pragma once
#include <atomic>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "LoopTimer.h"
#include "Rendering/ShaderProgram.h"
#include "Threading/ThreadQueueMeshes.h"
#include "World/Entities/EntityPosition.h"



class RenderingLoop
{
public:
	RenderingLoop();
	~RenderingLoop();

	void runLoop(std::shared_ptr<std::atomic<bool>> gameShouldClose, std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes);
private:

	LoopTimer framerateCounter;

	EntityPosition playerPos;

	GLFWwindow* mainWindow;
};