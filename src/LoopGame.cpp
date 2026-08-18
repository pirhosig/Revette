#include "LoopGame.h"

#include "GlobalLog.h"



const char* const NOISE_HEIGHTMAP = "FQkXCRUJDQAH@BCGZmBkAJBg@AIBEBAOamRk/C83MTD0EAg8JBg@AIBFBAOamRk/DAMAAKBBBAMAAEBBBA==";



LoopGame::LoopGame(
	GlobalApplicationState& _globalApplicationState,
	const Settings& settings,
	class GLFWwindow* _window
) :
	globalApplicationState{_globalApplicationState},
	world(globalApplicationState, settings, NOISE_HEIGHTMAP),
	player(EntityPosition({ 0.0, 150.0, 0.0 }), {0.8, 3.75, 0.8}),
	window{ _window }
{
	// Set user pointer for glfw callback functions
	glfwSetWindowUserPointer(window, this);

	// Disable visible cursor, and register a callback to capture mouse movement
	glfwSetCursorPosCallback(window, LoopGame::cursorPositionCallbackWrapper);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Get the current mouse position to avoid a larger jitter on the first frame
	glfwGetCursorPos(window, &cursorLastX, &cursorLastY);

	globalApplicationState.playerPosition.store(player.position);
	globalApplicationState.playerChunkPosition.store(ChunkPos(player.position));

	GlobalLog.Write("Created game loop");
}



void LoopGame::run() {
	double timeFrameLast = glfwGetTime();
	uint64_t currentTick = 0;

	// Run game until ESC is pressed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
		globalApplicationState.currentGameClockTick.wait(currentTick);
		currentTick++;

		double timeFrameBegin = glfwGetTime();
		double deltaTime = timeFrameBegin - timeFrameLast;
		timeFrameLast = timeFrameBegin;

		processInput(deltaTime);

		// Process the game events
		world.tick(player);

		globalApplicationState.playerPosition.store(player.position);
		globalApplicationState.playerChunkPosition.store(ChunkPos(player.position));
	}

	globalApplicationState.applicationShouldTerminate.store(true);
}



// Non-instance bound wrapper required for the glfw mouse callback
void LoopGame::cursorPositionCallbackWrapper(GLFWwindow* window, double xpos, double ypos) {
	LoopGame* instance = static_cast<LoopGame*>(glfwGetWindowUserPointer(window));
	instance->cursorPositionCallback(xpos, ypos);
}



void LoopGame::processInput(const double deltaTime) {
	glfwPollEvents();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	constexpr double PLAYER_SPEED_DEFAULT = 8.0;
	double _playerSpeed = PLAYER_SPEED_DEFAULT * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
		_playerSpeed *= 100.0;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		_playerSpeed *= 10.0;
	}
	_playerSpeed = std::clamp(_playerSpeed, 0.0, 32.0);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		player.position.displaceForward(_playerSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		player.position.displaceForward(-_playerSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		player.position.displaceSideways(-_playerSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		player.position.displaceSideways(_playerSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		player.position.displaceVertical(_playerSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		player.position.displaceVertical(-_playerSpeed);
	}
}



// This function is called every time the cursor position changes, and changes the camera rotation accordingly
void LoopGame::cursorPositionCallback(double xpos, double ypos) {
	// Get change in mouse position
	double deltaX = xpos - cursorLastX;
	double deltaY = ypos - cursorLastY;

	// Update the cursor position
	cursorLastX = xpos;
	cursorLastY = ypos;

	constexpr double sensitivity = 0.1;
	player.position.rotate(deltaX * sensitivity, -deltaY * sensitivity);
}
