#include "RenderingLoop.h"

#include <chrono>
#include <stdexcept>
#include <thread>
#include <iostream>

#include "Rendering/Renderer.h"

constexpr double PLAYER_SPEED_DEFAULT = 10.0;



RenderingLoop::RenderingLoop() : framerateCounter(1024), cursorLastX(0.0), cursorLastY(0.0)
{
	// Initialize glfw and configure it
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Get window information
	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);

	// Create a window and verify that it was created, then set it as the current opengl context on this thread
	mainWindow = glfwCreateWindow(videoMode->width, videoMode->height, "Revette-3D", NULL, NULL);
	if (mainWindow == NULL) throw std::runtime_error("Unable to create window");
	glfwMakeContextCurrent(mainWindow);

	// Load opengl function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::runtime_error("Failed to load openGL function pointers");

	// Set the user pointer for use in callback functions
	glfwSetWindowUserPointer(mainWindow, this);

	// Set the callback functions
	glfwSetCursorPosCallback(mainWindow, RenderingLoop::cursorPositionCallbackWrapper);

	// Disable the cursor
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwGetCursorPos(mainWindow, &cursorLastX, &cursorLastY);

	// Set the viewport to match the screen height and width
	glViewport(0, 0, videoMode->width, videoMode->height);
}



RenderingLoop::~RenderingLoop()
{
	glfwTerminate();
}



void RenderingLoop::runLoop(
	std::atomic<bool>& gameShouldClose,
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> threadQueueMeshes,
	std::shared_ptr<ThreadQueue<ChunkPos>> threadQueueMeshDeletion,
	std::atomic<PlayerState>& playerState
)
{
	Renderer gameRenderer(mainWindow, std::move(threadQueueMeshes), std::move(threadQueueMeshDeletion));
	auto lastFrame = std::chrono::steady_clock::now();

	while (!gameShouldClose.load())
	{
		const auto frameBegin = std::chrono::steady_clock::now();
		const auto frameEnd = frameBegin + std::chrono::milliseconds(15);
		const std::chrono::duration<double> timeElapsed = frameBegin - lastFrame;
		const double deltaTime = timeElapsed.count();
		lastFrame = frameBegin;

		// Exit if escape key is pressed
		if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			gameShouldClose.store(true);
			break;
		}

		processInput(deltaTime);

		gameRenderer.unloadMeshes(playerPos);
		gameRenderer.unqueueMeshes();
		gameRenderer.render(playerPos);

		playerState.store(PlayerState(playerPos));

		// Limit framerate
		std::this_thread::sleep_until(frameEnd);
		// Calculate framerate
		const auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - frameBegin);
		framerateCounter.addTime(frameTime.count());
	}
}



// Non-instance bound wrapper required for the glfw mouse callback
void RenderingLoop::cursorPositionCallbackWrapper(GLFWwindow* window, double xpos, double ypos)
{
	RenderingLoop* instance = static_cast<RenderingLoop*>(glfwGetWindowUserPointer(window));
	instance->cursorPositionCallback(xpos, ypos);
}



// Process keyboard and mouse input
void RenderingLoop::processInput(const double deltaTime)
{
	glfwPollEvents();
	double _playerSpeed = PLAYER_SPEED_DEFAULT;
	if (glfwGetKey(mainWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) _playerSpeed *= 10.0;
	if (glfwGetKey(mainWindow, GLFW_KEY_W)          == GLFW_PRESS) playerPos.moveForward(deltaTime * _playerSpeed);
	if (glfwGetKey(mainWindow, GLFW_KEY_S)          == GLFW_PRESS) playerPos.moveForward(deltaTime * -_playerSpeed);
	if (glfwGetKey(mainWindow, GLFW_KEY_A)          == GLFW_PRESS) playerPos.moveSideways(deltaTime * -_playerSpeed);
	if (glfwGetKey(mainWindow, GLFW_KEY_D)          == GLFW_PRESS) playerPos.moveSideways(deltaTime * _playerSpeed);
	if (glfwGetKey(mainWindow, GLFW_KEY_SPACE)      == GLFW_PRESS) playerPos.moveVertical(deltaTime * _playerSpeed);
	if (glfwGetKey(mainWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) playerPos.moveVertical(deltaTime * -_playerSpeed);
}



// This function is called every time the cursor position changes, and changes the camera rotation accordingly
void RenderingLoop::cursorPositionCallback(double xpos, double ypos)
{
	// Get change in mouse position
	double deltaX = xpos - cursorLastX;
	double deltaY = ypos - cursorLastY;
	// Update the mouse position
	cursorLastX = xpos;
	cursorLastY = ypos;

	constexpr double sensitivity = 0.1;
	playerPos.rotate(deltaX * sensitivity, -deltaY * sensitivity);
}

