#include "LoopGame.h"

#include "GlobalLog.h"



const char* NOISE_HEIGHTMAP = "GQAbABkAGwANAAYAAAB7FA5AEwAAAIA9CQAAmpkZPwDNzEy9AJqZGT8BJAAFAAAAEwDNzEw+FgABAAAA//8BAAAAAIBCAAAAAEE=";



LoopGame::LoopGame(
	GLFWwindow* _window,
	std::shared_ptr<ThreadPointerQueue<MeshDataChunk>> chunkMeshQueue,
	std::shared_ptr<ThreadQueue<ChunkPos>> chunkMeshQueueDeletion
) : timerFramerate(512),
	renderer(_window, chunkMeshQueue, chunkMeshQueueDeletion),
	world(chunkMeshQueue, chunkMeshQueueDeletion, NOISE_HEIGHTMAP),
	player(EntityPosition({ 0.0, 100.0, 0.0 }), {0.8, 3.75, 0.8}),
	window{ _window }
{
	// Set the user pointer for use in callback functions
	glfwSetWindowUserPointer(window, this);

	// Set the callback functions
	glfwSetCursorPosCallback(window, LoopGame::cursorPositionCallbackWrapper);

	// Disable the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the initial cursor position to avoid jump on first frame
	glfwGetCursorPos(window, &cursorLastX, &cursorLastY);

	GlobalLog.Write("Created game loop");
}



void LoopGame::run()
{
	double timeFrameLast = glfwGetTime();

	// Run game until ESC is pressed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		double timeFrameBegin = glfwGetTime();
		double deltaTime = timeFrameBegin - timeFrameLast;
		timeFrameLast = timeFrameBegin;

		// Update frametime counter
		timerFramerate.addTime(static_cast<long long>(deltaTime * 1000000.0));

		// Process input
		processInput(deltaTime);

		// Process the game events
		world.tick(player);

		// Render the frame
		renderer.unloadMeshes(ChunkPos(player));
		renderer.unqueueMeshes();
		renderer.render(player);
	}
}




// Non-instance bound wrapper required for the glfw mouse callback
void LoopGame::cursorPositionCallbackWrapper(GLFWwindow* window, double xpos, double ypos)
{
	LoopGame* instance = static_cast<LoopGame*>(glfwGetWindowUserPointer(window));
	instance->cursorPositionCallback(xpos, ypos);
}



void LoopGame::processInput(const double deltaTime)
{
	constexpr double PLAYER_SPEED_DEFAULT = 8.0;
	glfwPollEvents();
	double _playerSpeed = PLAYER_SPEED_DEFAULT * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) _playerSpeed *= 10.0;
	else if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) _playerSpeed *= 100.0;
	_playerSpeed = std::clamp(_playerSpeed, 0.0, 32.0);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) player.displaceForward( _playerSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) player.displaceForward(-_playerSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) player.displaceSideways(-_playerSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) player.displaceSideways( _playerSpeed);
	if (glfwGetKey(window, GLFW_KEY_SPACE     ) == GLFW_PRESS) player.displaceVertical( _playerSpeed);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) player.displaceVertical(-_playerSpeed);
}



// This function is called every time the cursor position changes, and changes the camera rotation accordingly
void LoopGame::cursorPositionCallback(double xpos, double ypos)
{
	// Get change in mouse position
	double deltaX = xpos - cursorLastX;
	double deltaY = ypos - cursorLastY;
	// Update the mouse position
	cursorLastX = xpos;
	cursorLastY = ypos;

	constexpr double sensitivity = 0.1;
	player.rotate(deltaX * sensitivity, -deltaY * sensitivity);
}


