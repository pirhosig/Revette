#include "RenderingLoop.h"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <iostream>

#include "Rendering/Renderer.h"

constexpr double PLAYER_SPEED = 3.0;



RenderingLoop::RenderingLoop() : framerateCounter(1024)
{
	// Initialize glfw and configure it
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Get window information
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

	mainWindow = glfwCreateWindow(videoMode->width, videoMode->height, "Revette-3D", primaryMonitor, NULL);
	if (mainWindow == NULL) throw std::runtime_error("Unable to create window");

	glfwMakeContextCurrent(mainWindow);


	// Load opengl function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::runtime_error("Failed to load openGL function pointers");

	// Sets the user pointer for use in callback functions
	glfwSetWindowUserPointer(mainWindow, this);

	glViewport(0, 0, videoMode->width, videoMode->height);
}



RenderingLoop::~RenderingLoop()
{
	glfwTerminate();
}



void RenderingLoop::runLoop(std::shared_ptr<std::atomic<bool>> gameShouldClose, std::shared_ptr<ThreadQueueMeshes> threadQueueMeshes)
{
	Renderer gameRenderer(mainWindow, std::move(threadQueueMeshes));
	auto lastFrame = std::chrono::steady_clock::now();

	while (!gameShouldClose->load())
	{
		const auto frameBegin = std::chrono::steady_clock::now();
		const auto frameEnd = frameBegin + std::chrono::milliseconds(15);
		const std::chrono::duration<double> timeElapsed = frameBegin - lastFrame;
		const double deltaTime = timeElapsed.count();
		lastFrame = frameBegin;

		glfwPollEvents();
		if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			gameShouldClose->store(true);
			break;
		}
		if (glfwGetKey(mainWindow, GLFW_KEY_W) == GLFW_PRESS) playerPos.moveForward(deltaTime * PLAYER_SPEED);
		if (glfwGetKey(mainWindow, GLFW_KEY_S) == GLFW_PRESS) playerPos.moveForward(deltaTime * -PLAYER_SPEED);
		if (glfwGetKey(mainWindow, GLFW_KEY_A) == GLFW_PRESS) playerPos.moveSideways(deltaTime * PLAYER_SPEED);
		if (glfwGetKey(mainWindow, GLFW_KEY_D) == GLFW_PRESS) playerPos.moveSideways(deltaTime * -PLAYER_SPEED);
		if (glfwGetKey(mainWindow, GLFW_KEY_SPACE) == GLFW_PRESS) playerPos.moveVertical(deltaTime * PLAYER_SPEED);
		if (glfwGetKey(mainWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) playerPos.moveVertical(deltaTime * -PLAYER_SPEED);

		gameRenderer.render(playerPos);

		// Limit framerate
		std::this_thread::sleep_until(frameEnd);
		// Calculate framerate
		const auto frameEndTime = std::chrono::steady_clock::now();
		const auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameBegin);
		framerateCounter.addTime(frameTime.count());
	}
}


